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

using namespace std::literals;

TEST(HelloTestSuite, HelloTest) {
    ASSERT_EQ(1, 1);
}

TEST(BasicFeatureTest, CanParseSingleOptional) {
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

TEST(BasicFeatureTest, CanParseSingleRequired) {
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