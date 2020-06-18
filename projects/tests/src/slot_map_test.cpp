#include <helios/containers/slot_map.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(SlotMap, DefaultConstructor)
{
    slot_map<i32> map;

    EXPECT_EQ(map.size(), 0);
    EXPECT_GE(map.capacity(), map.size());
    EXPECT_TRUE(map.empty());
}

TEST(SlotMap, CopyConstructor)
{
    slot_map<i32> orig;
    orig.insert(3);
    EXPECT_EQ(orig.size(), 1);
    EXPECT_GE(orig.capacity(), orig.size());

    auto it = orig.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, orig.end());
    EXPECT_FALSE(orig.empty());

    slot_map<i32> copy = orig;
    EXPECT_EQ(copy.size(), 1);
    EXPECT_GE(copy.capacity(), copy.size());

    it = copy.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, copy.end());
    EXPECT_FALSE(copy.empty());
}

TEST(SlotMap, MoveConstructor)
{
    slot_map<i32> orig;
    orig.insert(3);
    EXPECT_EQ(orig.size(), 1);
    EXPECT_GE(orig.capacity(), orig.size());

    auto it = orig.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, orig.end());
    EXPECT_FALSE(orig.empty());

    slot_map<i32> copy = helios::move(orig);
    EXPECT_EQ(copy.size(), 1);
    EXPECT_GE(copy.capacity(), copy.size());

    it = copy.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, copy.end());
    EXPECT_FALSE(copy.empty());
}

TEST(SlotMap, CopyAssignment)
{
    slot_map<i32> orig;
    orig.insert(3);
    EXPECT_EQ(orig.size(), 1);
    EXPECT_GE(orig.capacity(), orig.size());

    auto it = orig.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, orig.end());
    EXPECT_FALSE(orig.empty());

    slot_map<i32> copy;
    copy.insert(5);
    copy = orig;
    EXPECT_EQ(copy.size(), 1);
    EXPECT_GE(copy.capacity(), copy.size());

    it = copy.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, copy.end());
    EXPECT_FALSE(copy.empty());
}

TEST(SlotMap, MoveAssignment)
{
    slot_map<i32> orig;
    orig.insert(3);
    EXPECT_EQ(orig.size(), 1);
    EXPECT_GE(orig.capacity(), orig.size());

    auto it = orig.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, orig.end());
    EXPECT_FALSE(orig.empty());

    slot_map<i32> copy;
    copy.insert(5);
    copy = helios::move(orig);
    EXPECT_EQ(copy.size(), 1);
    EXPECT_GE(copy.capacity(), copy.size());

    it = copy.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, copy.end());
    EXPECT_FALSE(copy.empty());
}

TEST(SlotMap, InsertNoResize)
{
    slot_map<i32> m;
    for (u32 i = 0; i < 8; i++)
    {
        m.insert(i);
    }

    EXPECT_EQ(m.size(), 8);
    EXPECT_GE(m.capacity(), m.size());

    auto it = m.begin();
    EXPECT_EQ(*it, 0);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 7);
    ++it;
    EXPECT_EQ(it, m.end());

    EXPECT_FALSE(m.empty());
}

TEST(SlotMap, InsertWithResize)
{
    slot_map<i32> m;
    for (u32 i = 0; i < 10; i++)
    {
        m.insert(i);
    }

    EXPECT_EQ(m.size(), 10);
    EXPECT_GE(m.capacity(), m.size());

    auto it = m.begin();
    EXPECT_EQ(*it, 0);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 7);
    ++it;
    EXPECT_EQ(*it, 8);
    ++it;
    EXPECT_EQ(*it, 9);
    ++it;
    EXPECT_EQ(it, m.end());

    EXPECT_FALSE(m.empty());
}

TEST(SlotMap, Clear)
{
    slot_map<i32> map;
    auto it3 = map.insert(3);
    auto it4 = map.insert(4);
    map.clear();
    EXPECT_FALSE(map.erase(it3));
    EXPECT_FALSE(map.erase(it4));
}

TEST(SlotMap, Erase)
{
    slot_map<i32> map;
    auto it3 = map.insert(3);
    auto it4 = map.insert(4);
    EXPECT_TRUE(map.erase(it3));
    EXPECT_FALSE(map.contains(it3));
}

TEST(SlotMap, EraseThenInsert)
{
    slot_map<i32> map;
    auto it3 = map.insert(3);
    auto it4 = map.insert(4);
    EXPECT_TRUE(map.erase(it3));
    EXPECT_FALSE(map.contains(it3));
    auto it2 = map.insert(2);
    EXPECT_TRUE(map.contains(it2));
    EXPECT_FALSE(map.contains(it3));
}
