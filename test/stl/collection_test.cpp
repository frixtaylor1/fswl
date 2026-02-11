#include <gtest/gtest.h>
#include "../../src/stl/collection.hpp"

struct TestItem {
    int id;
    float value;
    bool operator==(const TestItem& other) const { return id == other.id; }
    bool operator!=(const TestItem& other) const { return id != other.id; }
};

template<typename T>
class CollectionTest : public ::testing::Test {
protected:
    static constexpr uint32 TEST_CAPACITY = 4;
    Collection<T, TEST_CAPACITY> collection;

    void SetUp() override {
    }

    void populate(uint32 count) {
        for (uint32 i = 0; i < count; ++i) {
            if constexpr (std::is_same_v<T, int>) {
                collection.add(static_cast<int>(i * 10));
            } else if constexpr (std::is_same_v<T, TestItem>) {
                collection.add({static_cast<int>(i), static_cast<float>(i * 1.5f)});
            }
        }
    }
};

using MyTypes = ::testing::Types<int, TestItem>;
TYPED_TEST_SUITE(CollectionTest, MyTypes);

TYPED_TEST(CollectionTest, InitializationAndCapacity) {
    ASSERT_TRUE(this->collection.initialized);
    ASSERT_TRUE(this->collection.isEmpty());
    EXPECT_EQ(this->collection.length, 0);
    EXPECT_EQ(this->collection.DEFAULT_CAPACITY, 4);
    EXPECT_FALSE(this->collection.isFull());
}

TYPED_TEST(CollectionTest, AddSingleItem) {
    TypeParam item_to_add{};
    this->collection.add(item_to_add);

    EXPECT_EQ(this->collection.length, 1);
    EXPECT_EQ(this->collection.at(0), item_to_add);
    EXPECT_FALSE(this->collection.isEmpty());
}

TYPED_TEST(CollectionTest, AddMultipleItems) {
    this->populate(3);
    
    EXPECT_EQ(this->collection.length, 3);
    EXPECT_FALSE(this->collection.isEmpty());
    EXPECT_FALSE(this->collection.isFull());
}

TYPED_TEST(CollectionTest, TryAddFullCapacity) {
    this->populate(this->TEST_CAPACITY);
    
    EXPECT_TRUE(this->collection.isFull());

    TypeParam overflow_item{};
    EXPECT_FALSE(this->collection.tryAdd(overflow_item)); 
    EXPECT_EQ(this->collection.length, this->TEST_CAPACITY);
}

TYPED_TEST(CollectionTest, AccessAt) {
    this->populate(2);
    
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(this->collection.at(1), 10);
    } else if constexpr (std::is_same_v<TypeParam, TestItem>) {
        EXPECT_EQ(this->collection.at(1).id, 1);
    }
}

TYPED_TEST(CollectionTest, RemoveMiddleItem) {
    this->populate(4);
    
    this->collection.removeAt(1);

    EXPECT_EQ(this->collection.length, 3);
    
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(this->collection.at(0), 0);
        EXPECT_EQ(this->collection.at(1), 20); 
        EXPECT_EQ(this->collection.at(2), 30);
    }
}

TYPED_TEST(CollectionTest, RemoveOnlyItem) {
    this->populate(1);
    this->collection.removeAt(0);

    EXPECT_TRUE(this->collection.isEmpty());
    EXPECT_EQ(this->collection.length, 0);
}

TYPED_TEST(CollectionTest, IteratorBasics) {
    this->populate(3);
    
    auto it = this->collection.begin();
    
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(*it, 0);
        EXPECT_EQ(*++it, 10);
        EXPECT_EQ(*it, 10);
    }

    it = this->collection.begin() + 2;
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(*it, 20);
    }
    
    auto start = this->collection.begin();
    auto end_it = this->collection.end();
    EXPECT_EQ(end_it - start, 3);
}

TYPED_TEST(CollectionTest, IndexOf) {
    this->populate(3);
    
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(this->collection.indexOf(10), 1); 
        EXPECT_EQ(this->collection.indexOf(99), -1); 
    } else if constexpr (std::is_same_v<TypeParam, TestItem>) {
        EXPECT_EQ(this->collection.indexOf({1, 0.0f}), 1); 
        EXPECT_EQ(this->collection.indexOf({99, 0.0f}), -1);
    }
}

TYPED_TEST(CollectionTest, EqualityOperator) {
    Collection<TypeParam, 4> other_collection;
    this->populate(2);
    other_collection.add(this->collection);

    EXPECT_TRUE(this->collection == other_collection);
    
    TypeParam different_item{};
    other_collection.add(different_item);

    EXPECT_TRUE(this->collection != other_collection);
}