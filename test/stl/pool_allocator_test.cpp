#include <gtest/gtest.h>

#include "../../src/stl/pool_allocator.hpp" 
#include <string.h>

class PoolAllocatorTest : public ::testing::Test {
protected:
    PoolAllocator allocator;

    void* allocate_and_check(uint bytes) {
        void* ptr = allocator.alloc(bytes);
        EXPECT_NE(ptr, nullptr) << "Failed to allocate " << bytes << " bytes.";
        return ptr;
    }
};

TEST_F(PoolAllocatorTest, BasicAllocationAndDeallocation) {
    uint size1 = 100;
    void* ptr1 = allocate_and_check(size1);
    
    uint size2 = 50;
    void* ptr2 = allocate_and_check(size2);
    
    allocator.dealloc(ptr1);
    allocator.dealloc(ptr2);
    
    uint size3 = 200;
    void* ptr3 = allocate_and_check(size3);
    
    allocator.dealloc(ptr3);
}

TEST_F(PoolAllocatorTest, BlockSplitting) {
    uint largeSize = 1024 * 1024 * 256; 
    
    void* ptrLarge = allocate_and_check(largeSize);
    
    uint smallSize = 100; 
    void* ptrSmall = allocate_and_check(smallSize);

    ASSERT_NE(ptrLarge, ptrSmall);
    
    PoolAllocator::Header* headerSmall = (PoolAllocator::Header*)ptrSmall - 1;
    ASSERT_TRUE(headerSmall->alloced);
    
    allocator.dealloc(ptrLarge);
    allocator.dealloc(ptrSmall);
}

TEST_F(PoolAllocatorTest, ReallocBehavior) {
    void* ptr1 = allocator.realloc(nullptr, 16);
    ASSERT_NE(ptr1, nullptr);
    
    const char data[] = "HOLA";
    uint oldSize = sizeof(data);
    ::memcpy(ptr1, data, oldSize);

    uint newSizeLarge = 64;
    void* ptr2 = allocator.realloc(ptr1, newSizeLarge);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr1, ptr2);
    
    ASSERT_EQ(::memcmp(ptr2, data, oldSize), 0);

    uint newSizeSmall = 2;
    void* ptr3 = allocator.realloc(ptr2, newSizeSmall);
    ASSERT_NE(ptr3, nullptr);
    ASSERT_NE(ptr2, ptr3);
    
    ASSERT_EQ(::memcmp(ptr3, "HO", newSizeSmall), 0);

    void* result = allocator.realloc(ptr3, 0);
    ASSERT_EQ(result, nullptr);
    
}

/* 
@TODO: Fix alloc ->

TEST_F(PoolAllocatorTest, OutOfMemory) {
    uint maxAllocSize = PoolAllocator::POOL_CAPACITY - 50;
    void* ptr = allocate_and_check(maxAllocSize);
    
    void* fail_ptr = allocator.alloc(100);
    
    ASSERT_EQ(fail_ptr, nullptr);
    
    allocator.dealloc(ptr);
} */

/* 
@TODO: fail case, double free detection:

TEST_F(PoolAllocatorTest, DoubleDeallocation) {
    void* ptr = allocate_and_check(10);
    
    allocator.dealloc(ptr);
    
    @TODO: here other dealloc to throw SA_ASSERT and terminate process

    EXPECT_DEATH(allocator.dealloc(ptr), "Double free detected");
}
 */