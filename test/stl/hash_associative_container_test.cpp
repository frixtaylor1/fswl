#include <gtest/gtest.h>
#include <set>
#include <map>
#include "../../src/stl/hash_associative_container.hpp"

template< class Key, class Value, uint CONTAINER_CAPACITY >
struct HashContainerWrapper {
    static constexpr uint CAPACITY = CONTAINER_CAPACITY;
    using ContainerType = HashAssociativeContainer< Key, Value, CAPACITY >;
};

using HashContainerTypes = ::testing::Types<HashContainerWrapper<int, float, 10>>;

template< typename T >
class HashAssociativeContainerTest : public ::testing::Test {
protected:
    using ContainerType = typename T::ContainerType;
    static constexpr uint TEST_CAPACITY = T::CAPACITY;
};

TYPED_TEST_SUITE(HashAssociativeContainerTest, HashContainerTypes);

TYPED_TEST(HashAssociativeContainerTest, AddAndExists) {
    using T = TypeParam;
    typename T::ContainerType container;

    float& val1 = container.add(1, 10.5f);
    
    ASSERT_EQ(container.length(), 1u);
    ASSERT_TRUE(container.exists(1));

    ASSERT_EQ(val1, 10.5f);

    container.add(5, 20.0f);
    ASSERT_EQ(container.length(), 2u);
    ASSERT_TRUE(container.exists(5));
    ASSERT_FALSE(container.exists(2));
}

TYPED_TEST(HashAssociativeContainerTest, AddExistingKeyReturnsReference) {
    using T = TypeParam;
    typename T::ContainerType container;

    float& initialVal = container.add(10, 50.0f);
    ASSERT_EQ(container.length(), 1u);

    float& existingValRef = container.add(10, 99.9f);
    
    ASSERT_EQ(container.length(), 1u);
    
    ASSERT_EQ(existingValRef, 99.9f); 

    existingValRef = 150.0f;
    
    ASSERT_EQ(container.at(10), 150.0f);
}

TYPED_TEST(HashAssociativeContainerTest, AccessValues) {
    using T = TypeParam;
    typename T::ContainerType container;

    container.add(100, 10.1f);
    container.add(200, 2.0f);
    container.add(300, 3.3f);

    ASSERT_EQ(container.at(100), 10.1f);
    ASSERT_EQ(container.getValue(200), 2.0f);
    ASSERT_EQ(container.getValue(300), 3.3f);
}

TYPED_TEST(HashAssociativeContainerTest, IteratorTest) {
    using T = TypeParam;
    typename T::ContainerType container;

    container.add(10, 100.0f);
    container.add(20, 200.0f);
    container.add(30, 300.0f);
    
    // Use a set to track visited elements (hash containers don't guarantee order)
    std::set<int> visitedKeys;
    std::map<int, float> expectedValues;
    expectedValues[10] = 100.0f;
    expectedValues[20] = 200.0f;
    expectedValues[30] = 300.0f;
    
    typename T::ContainerType::Iterator it;
    it.init(&container);
    
    uint count = 0;
    
    for (it.begin(); it.key() != nullptr && count < 3; it.next()) {
        int key = *it.key();
        float value = *it.value();
        
        // Verify the key is expected and value matches
        ASSERT_NE(expectedValues.find(key), expectedValues.end()) << "Unexpected key: " << key;
        ASSERT_EQ(value, expectedValues[key]);
        
        // Verify we don't visit the same key twice
        ASSERT_EQ(visitedKeys.count(key), 0u) << "Key visited multiple times: " << key;
        visitedKeys.insert(key);
        
        count++;
    }

    ASSERT_EQ(count, 3u);
    ASSERT_EQ(visitedKeys.size(), 3u);
}

TYPED_TEST(HashAssociativeContainerTest, CapacityLimit) {
    using T = TypeParam;
    typename T::ContainerType container;
    
    for (uint i = 0; i < T::CAPACITY; ++i) {
        container.add(i, (float)i);
    }

    ASSERT_EQ(container.length(), T::CAPACITY);
    
    float& valRef = container.add(0, 99.9f);
    ASSERT_EQ(container.length(), T::CAPACITY);
    ASSERT_EQ(valRef, 99.9f);
}

TYPED_TEST(HashAssociativeContainerTest, RemoveEntry) {
    using T = TypeParam;
    typename T::ContainerType container;

    container.add(1, 10.0f);
    container.add(2, 20.0f);
    container.add(3, 30.0f);

    ASSERT_EQ(container.length(), 3u);
    ASSERT_TRUE(container.exists(2));

    bool removed = container.remove(2);
    ASSERT_TRUE(removed);
    ASSERT_EQ(container.length(), 2u);
    ASSERT_FALSE(container.exists(2));

    bool notRemoved = container.remove(2);
    ASSERT_FALSE(notRemoved);
}

TYPED_TEST(HashAssociativeContainerTest, Clear) {
    using T = TypeParam;
    typename T::ContainerType container;

    container.add(1, 10.0f);
    container.add(2, 20.0f);
    container.add(3, 30.0f);

    ASSERT_EQ(container.length(), 3u);

    container.clear();
    ASSERT_EQ(container.length(), 0u);
    ASSERT_FALSE(container.exists(1));
    ASSERT_FALSE(container.exists(2));
    ASSERT_FALSE(container.exists(3));
}

TYPED_TEST(HashAssociativeContainerTest, HashCollisionHandling) {
    using T = TypeParam;
    typename T::ContainerType container;

    // Add values that might hash to similar buckets
    for (int i = 0; i < 5; ++i) {
        container.add(i * 128 + 1, (float)(i * 10));
    }

    ASSERT_EQ(container.length(), 5u);

    // Verify all values are still accessible
    for (int i = 0; i < 5; ++i) {
        ASSERT_TRUE(container.exists(i * 128 + 1));
        ASSERT_EQ(container.at(i * 128 + 1), (float)(i * 10));
    }
}
