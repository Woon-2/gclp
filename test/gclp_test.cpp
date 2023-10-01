/*
The MIT License

Copyright (c) 2023 Woon2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "gtest/gtest.h"
#include "cl_parser.hpp"
#include <string>
#include <string_view>
#include <tuple>

using namespace std::literals;

TEST(HelloTestSuite, HelloTest) {
    ASSERT_EQ(1, 1);
}

TEST(BasicParsingTest, ParseSingleOptional) {
    auto parser = clp::parser(
        "identifier"sv,
        clp::optional<int>(
            {'a'}, {"aa"}, "an optional int"
        )
    );

    auto [a] = parser.parse("identifier -a 3"sv);
    EXPECT_EQ(a, 3);
    auto [aa] = parser.parse("identifier --aa 4"sv);
    EXPECT_EQ(aa, 4);
}

TEST(BasicParsingTest, ParseSingleRequired) {
    auto parser = clp::parser(
        "identifier"sv,
        clp::required<int>(
            {'a'}, {"aa"}, "a required int"
        )
    );

    auto [a] = parser.parse("identifier -a 3"sv);
    EXPECT_EQ(a, 3);
    auto [aa] = parser.parse("identifier --aa 4"sv);
    EXPECT_EQ(aa, 4);
}

TEST(BasicParsingTest, ParseSingleBoolean) {
    auto parser = clp::parser(
        "identifier"sv,
        clp::optional<bool>(
            {'a'}, {"aa"}, "an optional boolean"
        )
    );
    auto [a1] = parser.parse("identifier -a 1"sv);
    EXPECT_TRUE(a1) << "parser doesn't recognize 1 as true.";
    auto [a2] = parser.parse("identifier -a true"sv);
    EXPECT_TRUE(a2) << "parser doesn't recognize \'true\' from command-line arguments.";
    auto [a3] = parser.parse("identifier -a"sv);
    EXPECT_TRUE(a3) << "parser doesn't recognize a boolean option with no argument as true.";
}

TEST(BasicParsingTest, ParseComplexBoolean) {
    auto parser = clp::parser(
        "identifier"sv,
        clp::optional<bool>( {'a'}, {"aa"}, "an optional boolean" ),
        clp::required<bool>( {'b'}, {"bb"}, "a required boolean" ),
        clp::optional<bool>( {'c'}, {"cc"}, "an optional boolean" )
    );

    auto [ra1, rb1, rc1] = parser.parse("identifer -abc"sv);

    EXPECT_TRUE(ra1);
    EXPECT_TRUE(rb1);
    EXPECT_TRUE(rc1);
}

TEST(BasicParsingTest, ParseComplexBooleanWithTwistedOrder) {
    auto parser = clp::parser(
        "identifier"sv,
        clp::optional<bool>( {'a'}, {"aa"}, "an optional boolean" ),
        clp::required<bool>( {'b'}, {"bb"}, "a required boolean" ),
        clp::optional<bool>( {'c'}, {"cc"}, "an optional boolean" )
    );

    auto [ra1, rb1, rc1] = parser.parse("identifer --bb -ac"sv);
    auto [ra2, rb2, rc2] = parser.parse("identifer -cba"sv);
    auto [ra3, rb3, rc3] = parser.parse("identifer -c -ba"sv);

    EXPECT_TRUE(ra1);
    EXPECT_TRUE(rb1);
    EXPECT_TRUE(rc1);
    EXPECT_TRUE(ra2);
    EXPECT_TRUE(rb2);
    EXPECT_TRUE(rc2);
    EXPECT_TRUE(ra3);
    EXPECT_TRUE(rb3);
    EXPECT_TRUE(rc3);
}

TEST(BasicParsingTest, FailWithParsingComplexBooleanContainingDuplication) {
    auto parser = clp::parser(
        "identifier"sv,
        clp::optional<bool>( {'a'}, {"aa"}, "an optional boolean" ),
        clp::required<bool>( {'b'}, {"bb"}, "a required boolean" ),
        clp::optional<bool>( {'c'}, {"cc"}, "an optional boolean" )
    );

    parser.parse("identifer -abcabc"sv);
    EXPECT_TRUE(parser.error());
}

TEST(SplitWordsTest, SplitWords) {
    auto splitted = clp::detail::split_words(
        "TestCLI -a 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1"sv
    );

    auto expected = decltype(splitted){
        "TestCLI"sv, "-a"sv, "1"sv, "-b"sv, "3.14"sv, "-c"sv,
        "c"sv, "-d"sv, "Hello"sv, "-e"sv, "World!"sv, "-f"sv,
        "1.6"sv, "-g"sv, "1"sv
    };

    EXPECT_EQ(splitted, expected);
}

TEST(SplitWordsTest, SplitWordsWithQuotes) {
    auto splitted = clp::detail::split_words(
        "TestCLI -a 1 -b 3.14 -c c -d \"Hello\" -e \"World!\" -f 1.6 -g 1"sv
    );

    auto expected = decltype(splitted){
        "TestCLI"sv, "-a"sv, "1"sv, "-b"sv, "3.14"sv, "-c"sv,
        "c"sv, "-d"sv, "Hello"sv, "-e"sv, "World!"sv, "-f"sv,
        "1.6"sv, "-g"sv, "1"sv
    };

    EXPECT_EQ(splitted, expected);
}

TEST(SplitWordsTest, SplitWordsWithQuotesContainingSpaces) {
    auto splitted = clp::detail::split_words(
        "TestCLI -a 1 -b 3.14 -c c -d \"Hello World!\" -e \"Bye World!\" -f 1.6 -g 1"sv
    );

    auto expected = decltype(splitted){
        "TestCLI"sv, "-a"sv, "1"sv, "-b"sv, "3.14"sv, "-c"sv,
        "c"sv, "-d"sv, "Hello World!"sv, "-e"sv, "Bye World!"sv, "-f"sv,
        "1.6"sv, "-g"sv, "1"sv
    };

    EXPECT_EQ(splitted, expected);
}

class ParsingTest : public ::testing::Test {
protected:
    ParsingTest()
        : a(
            {'a', 'A', 'i', 'I'},
            {"aa", "AA", "int", "integer", "Integer"},
            "an optional integer parameter"
        ),
        b(
            {'b', 'B', 'd', 'D'},
            {"bb", "BB", "double", "real", "Double", "Real"},
            "an optional real parameter"
        ),
        c(
            {'c', 'C'},
            {"cc", "CC", "char", "character", "Character"},
            "a required character parameter"
        ),
        d(
            {'d', 'D', 's', 'S'},
            {"dd", "DD", "string", "String"},
            "a required string parameter"
        ),
        e(
            {'e', 'E'},
            {"ee", "EE"},
            "an optional string parameter"
        ),
        f(
            {'f', 'F'},
            {"ff", "FF", "float", "Float"},
            "an optional float parameter"
        ),
        g(
            {'g', 'G', 'u', 'U'},
            {"gg", "GG", "unsigned_short", "UnsignedShort", "ushort"},
            "a required unsigned short parameter"
        ),
        h(
            {'h', 'H'},
            {"hh", "HH"},
            "an optional boolean parameter"
        ),
        i(
            {'i', 'I'},
            {"ii", "II"},
            "an optional boolean parameter"
        ),
        j(
            {'j', 'J'},
            {"jj", "JJ"},
            "an optional boolean parameter"
        ),
        k(
            {'k', 'K'},
            {"kk", "KK"},
            "an optional boolean parameter"
        ), 
        parser("TestCLI"sv, a, b, c, d, e, f, g, h, i, j, k) {}

private:
    clp::optional<int> a;
    clp::optional<double> b;
    clp::required<char> c;
    clp::required<std::string> d;
    clp::optional<std::string> e;
    clp::optional<float> f;
    clp::required<unsigned short> g;
    clp::optional<bool> h;
    clp::optional<bool> i;
    clp::required<bool> j;
    clp::required<bool> k;

protected:
    clp::parser<
        decltype(a), decltype(b), decltype(c),
        decltype(d), decltype(e), decltype(f),
        decltype(g), decltype(h), decltype(i),
        decltype(j), decltype(k)
    > parser;
};

TEST_F(ParsingTest, ParseMultipleArgs) {
    auto cmd = "TestCLI -a 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;
    auto res = parser.parse(cmd);

    ASSERT_FALSE(parser.error()) << parser.error_message();

    EXPECT_EQ(res, std::make_tuple(1, 3.14, 'c', "Hello"s, "World!"s, 1.6f, 1u, true ,true, true, true));
}

TEST_F(ParsingTest, OmitOptional) {
    auto cmd = "TestCLI -c c -d Hello -g 1 -j -k"sv;
    auto [ra, rb, rc, rd, re, rf, rg, rh, ri, rj, rk] = parser.parse(cmd);

    ASSERT_FALSE(parser.error()) << parser.error_message();

    EXPECT_EQ(rc, 'c');
    EXPECT_EQ(rd, "Hello");
    EXPECT_EQ(rg, 1u);
}

TEST_F(ParsingTest, FailWithOmittingRequired) {
    auto cmd = "TestCLI -a 1 -b 3.14 -e World! -f 1.6 -h -i"sv;
    auto [ra, rb, rc, rd, re, rf, rg, rh, ri, rj, rk] = parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != clp::error_code::required_key_not_given
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == clp::error_code::required_key_not_given
    ) << parser.error_message() << "\nparser doesn't detect required key not given.";
}

TEST_F(ParsingTest, IgnoreSpaceInQuoted) {
    auto cmd = "TestCLI -a 1 -b 3.14 -c c -d \"He llo\" -e \"Wo rld ! \" -f 1.6 -g 1 -h -i -j -k"sv;
    auto [ra, rb, rc, rd, re, rf, rg, rh, ri, rj ,rk] = parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != clp::error_code::unparsed_argument
    ) << parser.error_message();

    EXPECT_FALSE(parser.error() && parser.error()
        == clp::error_code::unparsed_argument
    ) << parser.error_message() << "\nremainders are from not ignoring space within quote.";
}

TEST_F(ParsingTest, CompareOverloadings) {
    int argc = 19;
    const char* argv[] = {"TestCLI", "-a", "1", "-b",
        "3.14", "-c", "c", "-d", "Hello", "-e", "World!",
        "-f", "1.6", "-g", "1", "-h", "-i", "-j", "-k"
    };

    auto cmd = "TestCLI -a 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    auto result_from_argc_argv = parser.parse(argc, argv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    auto result_from_string_view = parser.parse(cmd);
    ASSERT_FALSE(parser.error()) << parser.error_message();

    EXPECT_EQ(result_from_argc_argv, result_from_string_view)  
        << "overloadings of \"parse\" (argc, argv version/string view version) behaves differently.";
}

TEST_F(ParsingTest, FailWithWrongIdentifier) {
    auto cmd = "WrongCLI -a 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != clp::error_code::invalid_identifier
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == clp::error_code::invalid_identifier
    ) << parser.error_message() << "\nparser doesn't detect wrong identifier at the top priority.";
}

TEST_F(ParsingTest, FailWithSkippingKey) {
    auto cmd = "TestCLI 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != clp::error_code::key_not_given
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == clp::error_code::key_not_given
    ) << parser.error_message() << "\nparser doesn't detect wrong key-arguments order.";
}

TEST_F(ParsingTest, FailWithAssigningIncompatibleArgument) {
    auto cmd = "TestCLI -a abc -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != clp::error_code::incompatible_argument
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == clp::error_code::incompatible_argument
    ) << parser.error_message() << "\nparser doesn't detect assignment of incompatible arguments.\n"
        << "tried: assigning std::string to int";
}

TEST_F(ParsingTest, FailWithUndefinedKey) {
    auto cmd = "TestCLI -a 1 -b 3.14 -x c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != clp::error_code::undefined_key
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == clp::error_code::undefined_key
    ) << parser.error_message() << "\nparser doesn't detect usage of undefined key.\n";
}