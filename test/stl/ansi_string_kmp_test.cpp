#include <gtest/gtest.h>
#include "../../src/stl/ansi_string.hpp"

using uint = unsigned int;

TEST(AnsiStringKmpTest, SimplePattern) {
    AnsiString<16> s = "abababa";
    AnsiString<16> p = "aba";

    auto oc = s.ocurrencesOf(p);
    EXPECT_EQ(oc.length, 3);
    EXPECT_EQ(oc.at(0), 0);
    EXPECT_EQ(oc.at(1), 2);
    EXPECT_EQ(oc.at(2), 4);
}

TEST(AnsiStringKmpTest, NoMatch) {
    AnsiString<16> s = "abcdef";
    AnsiString<16> p = "xyz";
    auto oc = s.ocurrencesOf(p);
    EXPECT_EQ(oc.length, 0);
}

TEST(AnsiStringKmpTest, Overlapping) {
    AnsiString<16> s = "aaaaa";
    AnsiString<16> p = "aaa";
    auto oc = s.ocurrencesOf(p);
    EXPECT_EQ(oc.length, 3);
}

TEST(AnsiStringKmpTest, EdgeCases) {
    AnsiString<16> s = "";
    AnsiString<16> p = "a";
    auto oc = s.ocurrencesOf(p);
    EXPECT_EQ(oc.length, 0);

    AnsiString<16> s2 = "a";
    AnsiString<16> p2 = "";
    auto oc2 = s2.ocurrencesOf(p2);
    EXPECT_EQ(oc2.length, 0);
}