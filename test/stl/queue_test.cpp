#include <gtest/gtest.h>
#include "../../src/stl/queue.hpp"

TEST(QueueTest, EnqueueDequeueBasic) {
    Queue<int, 4> q;

    EXPECT_TRUE(q.isEmpty());
    EXPECT_EQ(q.length(), 0u);

    EXPECT_TRUE(q.enqueue(10));
    EXPECT_TRUE(q.enqueue(20));
    EXPECT_EQ(q.length(), 2u);
    EXPECT_FALSE(q.isEmpty());

    EXPECT_EQ(q.dequeue(), 10);
    EXPECT_EQ(q.dequeue(), 20);
    EXPECT_TRUE(q.isEmpty());
    EXPECT_EQ(q.length(), 0u);
}

TEST(QueueTest, WrapAround) {
    Queue<int, 4> q;

    EXPECT_TRUE(q.enqueue(1));
    EXPECT_TRUE(q.enqueue(2));
    EXPECT_TRUE(q.enqueue(3));

    EXPECT_TRUE(q.isFull());
    EXPECT_EQ(q.length(), 3u);

    EXPECT_EQ(q.dequeue(), 1);
    EXPECT_FALSE(q.isFull());

    EXPECT_TRUE(q.enqueue(4));
    EXPECT_TRUE(q.isFull());

    EXPECT_EQ(q.dequeue(), 2);
    EXPECT_EQ(q.dequeue(), 3);
    EXPECT_EQ(q.dequeue(), 4);
    EXPECT_TRUE(q.isEmpty());
}
