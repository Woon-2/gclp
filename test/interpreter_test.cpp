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
#include <vector>

using namespace std::literals;

TEST(RemoveDashTest, RemoveSingleDash) {
    auto src = "-abcdefg"sv;

    EXPECT_EQ(gclp::detail::remove_dash(src), "abcdefg"sv);
}

TEST(RemoveDashTest, RemoveDoubleDash) {
    auto src = "--abcdefg"sv;

    EXPECT_EQ(gclp::detail::remove_dash(src), "abcdefg"sv);
}

TEST(SplitWordsTest, SplitWords) {
    auto splitted = gclp::detail::split_words(
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
    auto splitted = gclp::detail::split_words(
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
    auto splitted = gclp::detail::split_words(
        "TestCLI -a 1 -b 3.14 -c c -d \"Hello World!\" -e \"Bye World!\" -f 1.6 -g 1"sv
    );

    auto expected = decltype(splitted){
        "TestCLI"sv, "-a"sv, "1"sv, "-b"sv, "3.14"sv, "-c"sv,
        "c"sv, "-d"sv, "Hello World!"sv, "-e"sv, "Bye World!"sv, "-f"sv,
        "1.6"sv, "-g"sv, "1"sv
    };

    EXPECT_EQ(splitted, expected);
}