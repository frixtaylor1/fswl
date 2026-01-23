#include <gtest/gtest.h>
#include "../../src/stl/associative_container.hpp"
template< class Key, class Value, uint CONTAINER_CAPACITY >
struct AssociativeContainerWrapper {
    static constexpr uint CAPACITY = CONTAINER_CAPACITY;
    using ContainerType = AssociativeContainer< Key, Value, CAPACITY >;
};

using ContainerTypes = ::testing::Types<AssociativeContainerWrapper<int, float, 10>>;

template< typename T >
class AssociativeContainerTest : public ::testing::Test {
protected:
    using ContainerType = typename T::ContainerType;
    static constexpr uint TEST_CAPACITY = T::CAPACITY;

    void SetUp() override {
    }
};

TYPED_TEST_SUITE(AssociativeContainerTest, ContainerTypes);

TYPED_TEST(AssociativeContainerTest, AddAndExists) {
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

TYPED_TEST(AssociativeContainerTest, AddExistingKeyReturnsReference) {
    using T = TypeParam;
    typename T::ContainerType container;

    float& initialVal = container.add(10, 50.0f);
    ASSERT_EQ(container.length(), 1u);

    float& existingValRef = container.add(10, 99.9f);
    
    ASSERT_EQ(container.length(), 1u);
    
    ASSERT_EQ(existingValRef, 50.0f); 

    existingValRef = 150.0f;
    
    ASSERT_EQ(container.at(10), 150.0f);
}

TYPED_TEST(AssociativeContainerTest, AccessValues) {
    using T = TypeParam;
    typename T::ContainerType container;

    container.add(100, 1.0f);
    container.add(200, 2.0f);

    ASSERT_EQ(container.at(100), 1.0f);
    ASSERT_EQ(container.getValue(200), 2.0f);

    container.at(100) = 10.1f;
    ASSERT_EQ(container.getValue(100), 10.1f);

    ASSERT_EQ(container.getKeyAt(0), 100);
    ASSERT_EQ(container.getValueAt(1), 2.0f);
}

TYPED_TEST(AssociativeContainerTest, IteratorTest) {
    using T = TypeParam;
    typename T::ContainerType container;

    container.add(10, 100.0f);
    container.add(20, 200.0f);
    container.add(30, 300.0f);
    
    std::vector<std::pair<int, float>> expected = {
        {10, 100.0f},
        {20, 200.0f},
        {30, 300.0f}
    };
    
    typename T::ContainerType::Iterator it;
    it.init(&container);
    
    uint count = 0;
    
    for (it.begin(); *it.key() != *it.end().key(); it.next()) {
        ASSERT_EQ(*it.key(), expected[count].first);
        ASSERT_EQ(*it.value(), expected[count].second);
        count++;
    }

    ASSERT_EQ(count, 3u);
}

TYPED_TEST(AssociativeContainerTest, CapacityLimit) {
    using T = TypeParam;
    typename T::ContainerType container;
    
    for (uint i = 0; i < T::CAPACITY; ++i) {
        container.add(i, (float)i);
    }

    ASSERT_EQ(container.length(), T::CAPACITY);
    
    float& valRef = container.add(0, 99.9f);
    ASSERT_EQ(container.length(), T::CAPACITY);
    ASSERT_EQ(valRef, 0.0f);
}