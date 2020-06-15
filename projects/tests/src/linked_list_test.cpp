#include <helios/containers/linked_list.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(LinkedList, DefaultConstructor)
{
    linked_list<i32> list;
    EXPECT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

TEST(LinkedList, InitializerListConstructor)
{
    linked_list<i32> list = {1, 2, 3};
    EXPECT_EQ(list.size(), 3);
    ASSERT_FALSE(list.empty());

    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
}
