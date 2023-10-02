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

TEST(ParamTest, SetValue) {
    auto paramI = gclp::optional<int>(
        {'i'}, {"integer"}, "an optional integer parameter"
    );

    auto paramS = gclp::required<std::string>(
        {'s'}, {"string"}, "a required string parameter"
    );

    paramI.set_value(42);
    paramS.set_value("Hello, World!");

    ASSERT_TRUE(paramI.has_value());
    ASSERT_TRUE(paramS.has_value());

    EXPECT_EQ(paramI.value(), 42);
    EXPECT_EQ(paramS.value(), "Hello, World!");
}

TEST(ParamTest, RemoveValue) {
    auto paramI = gclp::optional<int>(
        {'i'}, {"integer"}, "an optional integer parameter"
    );

    auto paramS = gclp::required<std::string>(
        {'s'}, {"string"}, "a required string parameter"
    );

    paramI.set_value(42);
    paramS.set_value("Hello, World!");

    paramI.remove_value();
    paramS.remove_value();

    EXPECT_FALSE(paramI.has_value());
    EXPECT_FALSE(paramS.has_value());
}

TEST(ParamTest, SetDefaultValue) {
    auto paramI = gclp::optional<int>(
        {'i'}, {"integer"}, "an optional integer parameter"
    )->defval(42);

    auto paramS = gclp::required<std::string>(
        {'s'}, {"string"}, "a required string parameter"
    )->defval("Hello, World!");

    ASSERT_TRUE(paramI.has_defval());
    ASSERT_TRUE(paramS.has_defval());

    EXPECT_EQ(paramI.get_defval(), 42);
    EXPECT_EQ(paramS.get_defval(), "Hello, World!");
}

TEST(ParamTest, RemoveDefaultValue) {
    auto paramI = gclp::optional<int>(
        {'i'}, {"integer"}, "an optional integer parameter"
    )->defval(42);

    auto paramS = gclp::required<std::string>(
        {'s'}, {"string"}, "a required string parameter"
    )->defval("Hello, World!");

    paramI.remove_defval();
    paramS.remove_defval();

    EXPECT_FALSE(paramI.has_defval());
    EXPECT_FALSE(paramS.has_defval());
}