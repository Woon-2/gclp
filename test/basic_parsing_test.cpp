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

using namespace std::literals;

TEST(BasicParsingTest, ParseJustIdentifier) {
    auto parser = gclp::parser(
        "identifier"sv
    );
    auto res = parser.parse("identifier"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
}

TEST(BasicParsingTest, ParseSingleOptional) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::optional<int>(
            {'a'}, {"aa"}, "an optional int"
        )
    );

    auto [a] = parser.parse("identifier -a 3"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_EQ(a, 3);
    auto [aa] = parser.parse("identifier --aa 4"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_EQ(aa, 4);
}

TEST(BasicParsingTest, ParseSingleRequired) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::required<int>(
            {'a'}, {"aa"}, "a required int"
        )
    );

    auto [a] = parser.parse("identifier -a 3"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_EQ(a, 3);
    auto [aa] = parser.parse("identifier --aa 4"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_EQ(aa, 4);
}

TEST(BasicParsingTest, ParseSingleBoolean) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::optional<bool>(
            {'a'}, {"aa"}, "an optional boolean"
        )
    );
    auto [a1] = parser.parse("identifier -a 1"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_TRUE(a1) << "parser doesn't recognize 1 as true.\n";
    auto [a2] = parser.parse("identifier -a true"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_TRUE(a2) << "parser doesn't recognize \'true\' from command-line arguments.\n";
    auto [a3] = parser.parse("identifier -a"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    EXPECT_TRUE(a3) << "parser doesn't recognize a boolean option with no argument as true.\n";
}

TEST(BasicParsingTest, ParseComplexBoolean) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::optional<bool>( {'a'}, {"aa"}, "an optional boolean" ),
        gclp::required<bool>( {'b'}, {"bb"}, "a required boolean" ),
        gclp::optional<bool>( {'c'}, {"cc"}, "an optional boolean" )
    );

    auto [ra1, rb1, rc1] = parser.parse("identifier -abc"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();

    EXPECT_TRUE(ra1);
    EXPECT_TRUE(rb1);
    EXPECT_TRUE(rc1);
}

TEST(BasicParsingTest, ParseComplexBooleanWithTwistedOrder) {
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::optional<bool>( {'a'}, {"aa"}, "an optional boolean" ),
        gclp::required<bool>( {'b'}, {"bb"}, "a required boolean" ),
        gclp::optional<bool>( {'c'}, {"cc"}, "an optional boolean" )
    );

    auto [ra1, rb1, rc1] = parser.parse("identifier --bb -ac"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    auto [ra2, rb2, rc2] = parser.parse("identifier -cba"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();
    auto [ra3, rb3, rc3] = parser.parse("identifier -c -ba"sv);
    ASSERT_FALSE(parser.error()) << parser.error_message();

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
    auto parser = gclp::parser(
        "identifier"sv,
        gclp::optional<bool>( {'a'}, {"aa"}, "an optional boolean" ),
        gclp::required<bool>( {'b'}, {"bb"}, "a required boolean" ),
        gclp::optional<bool>( {'c'}, {"cc"}, "an optional boolean" )
    );

    parser.parse("identifier -abcabc"sv);
    ASSERT_FALSE(parser.error() && parser.error()
        != gclp::error_code::duplicated_assignments
    ) << parser.error_message();
    EXPECT_TRUE(parser.error() && parser.error()
        == gclp::error_code::duplicated_assignments
    ) << "parser doesn't detect duplicated assignments occured within complex boolean params\n";
}