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