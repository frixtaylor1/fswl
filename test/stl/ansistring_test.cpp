#include <gtest/gtest.h>
#include "../../src/stl/ansi_string.hpp" 

using uint = unsigned int;

template<uint STR_CAPACITY>
struct AnsiStringWrapper {
    static constexpr uint CAPACITY = STR_CAPACITY;
    using StringType = AnsiString<CAPACITY>;
};

using AnsiStringTypes = ::testing::Types<
    AnsiStringWrapper<16>,
    AnsiStringWrapper<256>
>;

template<typename T>
class AnsiStringTest : public ::testing::Test {
protected:
    using StringType = typename T::StringType;
    static constexpr uint TEST_CAPACITY = T::CAPACITY;

    StringType empty_string;
    StringType short_string;
    const char* test_literal = "TestString";

    void SetUp() override {
        short_string.init(test_literal);
    }
};

TYPED_TEST_SUITE(AnsiStringTest, AnsiStringTypes);

TYPED_TEST(AnsiStringTest, DefaultInitialization) {
    EXPECT_EQ(this->empty_string.length(), 0);
    EXPECT_STREQ(this->empty_string.cstr(), "");
    EXPECT_EQ(this->empty_string.MAX_STR_CAP, this->TEST_CAPACITY); 
}

TYPED_TEST(AnsiStringTest, LiteralInitialization) {
    EXPECT_EQ(this->short_string.length(), strlen(this->test_literal));
    EXPECT_STREQ(this->short_string.cstr(), this->test_literal);
}

TYPED_TEST(AnsiStringTest, AssignmentOperator) {
    const char* new_literal = "NewValue";
    this->empty_string = new_literal;
    EXPECT_EQ(this->empty_string.length(), strlen(new_literal));
    EXPECT_STREQ(this->empty_string.cstr(), new_literal);
}

TYPED_TEST(AnsiStringTest, ConcatPlainString) {
    const char* suffix = "Concat";
    this->short_string.concat(suffix);
    const char* expected = "TestStringConcat";
    EXPECT_EQ(this->short_string.length(), strlen(expected));
    EXPECT_STREQ(this->short_string.cstr(), expected);
}

TYPED_TEST(AnsiStringTest, ConcatChar) {
    this->short_string.concat('X');
    const char* expected = "TestStringX";
    EXPECT_EQ(this->short_string.length(), strlen(expected));
    EXPECT_STREQ(this->short_string.cstr(), expected);
}

TYPED_TEST(AnsiStringTest, ConcatAnsiString) {
    typename TestFixture::StringType other_string;
    other_string = "Suffix";
    this->short_string.concat(other_string);
    const char* expected = "TestStringSuffix";
    EXPECT_EQ(this->short_string.length(), strlen(expected));
    EXPECT_STREQ(this->short_string.cstr(), expected);
}

TYPED_TEST(AnsiStringTest, TrimSpaces) {
    typename TestFixture::StringType padded_string;
    padded_string = " \t Hello World \n";
    padded_string.trim();
    EXPECT_STREQ(padded_string.cstr(), "\t Hello World \n");
    EXPECT_EQ(padded_string.length(), 15);
}

TYPED_TEST(AnsiStringTest, AccessAt) {
    EXPECT_EQ(this->short_string.at(0), 'T');
    EXPECT_EQ(this->short_string.at(4), 'S');
    EXPECT_EQ(this->short_string.at(9), 'g');
}

TYPED_TEST(AnsiStringTest, PosChar) {
    EXPECT_EQ(this->short_string.pos('t'), 3);
    EXPECT_EQ(this->short_string.pos('S'), 4);
    EXPECT_EQ(this->short_string.pos('Z'), -1);
}

TYPED_TEST(AnsiStringTest, PosPattern) {
    EXPECT_EQ(this->short_string.pos("String"), 4);
    EXPECT_EQ(this->short_string.pos("Test"), 0);
    EXPECT_EQ(this->short_string.pos("NonExistent"), -1);
}

TYPED_TEST(AnsiStringTest, PosAnsiString) {
    typename TestFixture::StringType pattern_string;
    pattern_string = "ing";
    EXPECT_EQ(this->short_string.pos(pattern_string), 7);
    pattern_string = "Mismatch";
    EXPECT_EQ(this->short_string.pos(pattern_string), -1);
}

TYPED_TEST(AnsiStringTest, EqualityOperator) {
    typename TestFixture::StringType same_string;
    same_string = this->test_literal;
    
    EXPECT_TRUE(this->short_string == same_string);
    EXPECT_TRUE(this->short_string.equals(same_string));

    typename TestFixture::StringType different_string;
    different_string = "Different";
    EXPECT_TRUE(this->short_string != different_string);
}

TYPED_TEST(AnsiStringTest, ComparativeOperators) {
    typename TestFixture::StringType smaller = "Apple";
    typename TestFixture::StringType greater = "Zebrass";
    typename TestFixture::StringType middle = "Banana";

    EXPECT_TRUE(middle.lessThan(this->short_string));
    EXPECT_FALSE(middle.greatherThan(this->short_string));
    
    EXPECT_TRUE(this->short_string.lessOrEqualThan(this->short_string));
    EXPECT_TRUE(this->short_string.greatherOrEqualThan(this->short_string));
    
    EXPECT_TRUE(smaller.lessThan(greater));
    EXPECT_TRUE(greater.greatherThan(smaller));
}

TYPED_TEST(AnsiStringTest, ToLower) {
    typename TestFixture::StringType mixed_case;
    mixed_case = "MiXeD CaSe";
    mixed_case.toLower();
    EXPECT_STREQ(mixed_case.cstr(), "mixed case");
}

TYPED_TEST(AnsiStringTest, ToUpper) {
    typename TestFixture::StringType mixed_case;
    mixed_case = "MiXeD CaSe";
    mixed_case.toUpper();
    EXPECT_STREQ(mixed_case.cstr(), "MIXED CASE");
}

TYPED_TEST(AnsiStringTest, SubStr) {
    auto sub = this->short_string.template subStr<6>(4, 10);
    EXPECT_STREQ(sub.cstr(), "String");
    EXPECT_EQ(sub.length(), 6);
    
    auto sub2 = this->short_string.template subStr<4>(0, 4);
    EXPECT_STREQ(sub2.cstr(), "Test");
}

TYPED_TEST(AnsiStringTest, Split) {
    AnsiString list_string = "item1,item2,item3,item4";
    
    auto collection = list_string.split(',');
    
    EXPECT_EQ(collection.length, 4); 

    EXPECT_STREQ(collection.at(0).cstr(), "item1");
    EXPECT_STREQ(collection.at(1).cstr(), "item2");
    EXPECT_STREQ(collection.at(2).cstr(), "item3");
    EXPECT_STREQ(collection.at(3).cstr(), "item4");
}

TYPED_TEST(AnsiStringTest, StaticToStringInt) {
    auto str = TestFixture::StringType::toString(12345);
    EXPECT_STREQ(str.cstr(), "12345");
}

TYPED_TEST(AnsiStringTest, StaticToStringFloat) {
    auto str = TestFixture::StringType::toString(1.25f);
    EXPECT_TRUE(str.length() > 0);
}

TYPED_TEST(AnsiStringTest, OcurrencesOfChar) {
    typename TestFixture::StringType string_with_duplicates;
    string_with_duplicates = "abracadabra";
    
    auto positions_a = string_with_duplicates.ocurrencesOf('a');
    EXPECT_EQ(positions_a.length, 5); // 0, 3, 5, 7, 10
    
    auto positions_b = string_with_duplicates.ocurrencesOf('b');
    EXPECT_EQ(positions_b.length, 2); // 1, 8
}

/** 
 * @TODO FIX OcurrencesOfChar... OcurrencesOfPattern 
 TYPED_TEST(AnsiStringTest, OcurrencesOfPattern) {
    typename TestFixture::StringType string_with_pattern;
    string_with_pattern = "abababa";
    typename TestFixture::StringType pattern;
    pattern = "aba";
    
    auto positions = string_with_pattern.ocurrencesOf(pattern);
    EXPECT_EQ(positions.length, 3); // 0, 2, 4
    
    // Asumiendo que Collection<int>::at() funciona
    // EXPECT_EQ(positions.at(0), 0);
    // EXPECT_EQ(positions.at(1), 2);
    // EXPECT_EQ(positions.at(2), 4);
} 
*/