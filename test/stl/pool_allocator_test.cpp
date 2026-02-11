#include <gtest/gtest.h>

#include "../../src/stl/pool_allocator.hpp" 
#include <string.h>

class PoolAllocatorTest : public ::testing::Test {
protected:
    PoolAllocator allocator;

    void* allocate_and_check(uint32 bytes) {
        void* ptr = allocator.alloc(bytes);
        EXPECT_NE(ptr, nullptr) << "Failed to allocate " << bytes << " bytes.";
        return ptr;
    }
};

TEST_F(PoolAllocatorTest, BlockSplitting) {
    uint32 largeSize = 1024 * 1024 * 256; 
    
    void* ptrLarge = allocate_and_check(largeSize);
    
    uint32 smallSize = 100; 
    void* ptrSmall = allocate_and_check(smallSize);

    ASSERT_NE(ptrLarge, ptrSmall);
    
    PoolAllocator::Header* headerSmall = (PoolAllocator::Header*)ptrSmall - 1;
    ASSERT_TRUE(headerSmall->alloced);
    
    allocator.dealloc(ptrLarge);
    allocator.dealloc(ptrSmall);
}

/** @TODO: MUST FIX
TEST_F(PoolAllocatorTest, ReallocBehavior) {
    void* ptr1 = allocator.realloc(nullptr, 16);
    ASSERT_NE(ptr1, nullptr);
    
    const char data[] = "HOLA";
    uint32 oldSize = sizeof(data);
    ::memcpy(ptr1, data, oldSize);

    uint32 newSizeLarge = 64;
    void* ptr2 = allocator.realloc(ptr1, newSizeLarge);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr1, ptr2);
    
    ASSERT_EQ(::memcmp(ptr2, data, oldSize), 0);

    uint32 newSizeSmall = 2;
    void* ptr3 = allocator.realloc(ptr2, newSizeSmall);
    ASSERT_NE(ptr3, nullptr);
    ASSERT_NE(ptr2, ptr3);
    
    ASSERT_EQ(::memcmp(ptr3, "HO", newSizeSmall), 0);

    void* result = allocator.realloc(ptr3, 0);
    ASSERT_EQ(result, nullptr);
} */

TEST_F(PoolAllocatorTest, BasicDeallocation) {
    uint32 maxAllocSize = PoolAllocator::POOL_CAPACITY - 50;
    void* ptr = allocate_and_check(maxAllocSize);
    void* fail_ptr = allocator.alloc(100);
    const PoolAllocator::Header* header = allocator.inspectHeader(ptr);

    allocator.dealloc(ptr);
    
    ASSERT_FALSE(header->alloced);
}

TEST_F(PoolAllocatorTest, DoubleDeallocation) {
    void* ptr = allocate_and_check(10);
    
    allocator.dealloc(ptr);
    EXPECT_THROW(allocator.dealloc(ptr), std::runtime_error);
}