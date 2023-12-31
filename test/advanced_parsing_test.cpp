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
#include "gclp.hpp"
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

using namespace std::literals;

class ParsingTest : public ::testing::Test {
protected:
    ParsingTest()
        : a(
            {'a', 'A'},
            {"aa", "AA", "int", "integer", "Integer"},
            "an optional integer parameter"
        ),
        b(
            {'b', 'B'},
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
    gclp::optional<int> a;
    gclp::optional<double> b;
    gclp::required<char> c;
    gclp::required<std::string> d;
    gclp::optional<std::string> e;
    gclp::optional<float> f;
    gclp::required<unsigned short> g;
    gclp::optional<bool> h;
    gclp::optional<bool> i;
    gclp::required<bool> j;
    gclp::required<bool> k;

protected:
    gclp::parser<
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
        != gclp::error_code::required_key_not_given
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::required_key_not_given
    ) << parser.error_message() << "\nparser doesn't detect required key not given.\n";
}

TEST_F(ParsingTest, IgnoreSpaceInQuoted) {
    auto cmd = "TestCLI -a 1 -b 3.14 -c c -d \"He llo\" -e \"Wo rld ! \" -f 1.6 -g 1 -h -i -j -k"sv;
    auto [ra, rb, rc, rd, re, rf, rg, rh, ri, rj ,rk] = parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::unparsed_argument
    ) << parser.error_message();

    EXPECT_FALSE(parser.error() && parser.error()
        == gclp::error_code::unparsed_argument
    ) << parser.error_message() << "\nremainders are from not ignoring space within quote.\n";
}

TEST_F(ParsingTest, CompareOverloadings) {
    int argc = 19;
    const char* const argv[] = {"TestCLI", "-a", "1", "-b",
        "3.14", "-c", "c", "-d", "Hello", "-e", "World!",
        "-f", "1.6", "-g", "1", "-h", "-i", "-j", "-k"
    };

    auto cmd = "TestCLI -a 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    auto result_from_argc_argv = parser.parse(argc, argv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    auto result_from_string_view = parser.parse(cmd);
    ASSERT_FALSE(parser.error()) << parser.error_message();

    EXPECT_EQ(result_from_argc_argv, result_from_string_view)  
        << "overloadings of \"parse\" (argc, argv version/string view version) behaves differently.\n";
}

TEST_F(ParsingTest, FailWithWrongIdentifier) {
    auto cmd = "WrongCLI -a 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::invalid_identifier
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::invalid_identifier
    ) << parser.error_message() << "\nparser doesn't detect wrong identifier at the top priority.\n";
}

TEST_F(ParsingTest, FailWithSkippingKey) {
    auto cmd = "TestCLI 1 -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::key_not_given
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::key_not_given
    ) << parser.error_message() << "\nparser doesn't detect wrong key-arguments order.\n";
}

TEST_F(ParsingTest, FailWithAssigningIncompatibleArgument) {
    auto cmd = "TestCLI -a abc -b 3.14 -c c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::incompatible_argument
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::incompatible_argument
    ) << parser.error_message() << "\nparser doesn't detect assignment of incompatible arguments.\n"
        << "tried: assigning std::string to int";
}

TEST_F(ParsingTest, FailWithUndefinedKey) {
    auto cmd = "TestCLI -a 1 -b 3.14 -x c -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::undefined_key
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::undefined_key
    ) << parser.error_message() << "\nparser doesn't detect usage of undefined key.\n";
}

TEST_F(ParsingTest, FailWithAssignmentDuplicationOfExactSameKeys) {
    auto cmd = "TestCLI -a 1 -b 3.14 -c c -a 2 -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::duplicated_assignments
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::duplicated_assignments
    ) << parser.error_message() << "\nparser doesn't detect assignment duplication of exact same keys.\n";
}

TEST_F(ParsingTest, FailWithAssignmentDuplicationOfDifferentKeys) {
    auto cmd = "TestCLI -a 1 -b 3.14 -c c --AA 2 -d Hello -e World! -f 1.6 -g 1 -h -i -j -k"sv;

    parser.parse(cmd);

    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::duplicated_assignments
    ) << parser.error_message();

    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::duplicated_assignments
    ) << parser.error_message() << "\nparser doesn't detect assignment duplication of different keys.\n";
}

TEST_F(ParsingTest, AssignDefaultValue) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::optional<int>(
            {'a'}, {"aa"}, "an optional int"
        )->defval(3),
        gclp::required<std::string>(
            {'b'}, {"bb"}, "a required string"
        )->defval("Hello, World!")
    );

    auto [ra, rb] = parser.parse("identifier"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_EQ(ra, 3);
    EXPECT_EQ(rb, "Hello, World!");
}

TEST_F(ParsingTest, ConserveTypeWhenUsingDefaultValueAdaptor) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::required<int>(
            {'a'}, {"aa"}, "an optional int"
        )->defval(3),
        gclp::optional<std::string>(
            {'b'}, {"bb"}, "a required string"
        )->defval("Hello, World!")
    );

    auto is_type_conserved = [&parser]() {
        using none_qualified_t = std::remove_cvref_t<decltype(parser)>;
        if ( !std::is_same_v<
            none_qualified_t::param_tuple_type,
            std::tuple<
                gclp::required<int>,
                gclp::optional<std::string>
            >
        >) {
            return ::testing::AssertionFailure();
        }
        else {
            return ::testing::AssertionSuccess();
        }
    };

    EXPECT_TRUE(is_type_conserved());
}