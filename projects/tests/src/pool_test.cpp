#include <helios/containers/pool.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(MemoryPool, DefaultConstructor)
{
    memory_pool<i32, 4> pool;
    EXPECT_EQ(pool.size(), 0);
    EXPECT_GE(pool.capacity(), pool.size());
}

TEST(MemoryPool, CopyConstructor)
{
    memory_pool<i32, 4> pool;
    pool.resize(1);
    pool.set(3, 1);
    EXPECT_EQ(pool.size(), 4);
    EXPECT_GE(pool.capacity(), pool.size());
    EXPECT_EQ(pool.at(3), 1);

    memory_pool<i32, 4> copy = pool;
    EXPECT_EQ(copy.size(), 4);
    EXPECT_GE(copy.capacity(), pool.size());
    EXPECT_EQ(copy.at(3), 1);
}

TEST(MemoryPool, MoveConstructor)
{
    memory_pool<i32, 4> pool;
    pool.resize(1);
    pool.set(3, 1);
    EXPECT_EQ(pool.size(), 4);
    EXPECT_GE(pool.capacity(), pool.size());
    EXPECT_EQ(pool.at(3), 1);

    memory_pool<i32, 4> copy = helios::move(pool);
    EXPECT_EQ(copy.size(), 4);
    EXPECT_GE(copy.capacity(), pool.size());
    EXPECT_EQ(copy.at(3), 1);
}

TEST(MemoryPool, FillAndResize)
{
    memory_pool<i32, 4> pool;
    pool.resize(1);
    pool.set(3, 1);
    EXPECT_EQ(pool.size(), 4);
    EXPECT_GE(pool.capacity(), pool.size());
    EXPECT_EQ(pool.at(3), 1);

    pool.resize(2);
    EXPECT_EQ(pool.size(), 4);
    EXPECT_GE(pool.capacity(), pool.size());

    pool.set(5, 4);
    EXPECT_EQ(pool.size(), 6);
    EXPECT_GE(pool.capacity(), pool.size());

    EXPECT_EQ(pool.at(3), 1);
    EXPECT_EQ(pool.at(5), 4);
}
