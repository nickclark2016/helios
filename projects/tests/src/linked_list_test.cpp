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

TEST(LinkedList, CopyConstructor)
{
    linked_list<i32> orig = {1, 2, 3};
    linked_list<i32> copy = orig;
    auto origIt = orig.begin();
    auto copyIt = copy.begin();

    EXPECT_EQ(orig.size(), copy.size());
    EXPECT_EQ(*origIt, *copyIt);
    ++origIt;
    ++copyIt;
    EXPECT_EQ(*origIt, *copyIt);
    ++origIt;
    ++copyIt;
    EXPECT_EQ(*origIt, *copyIt);
}

TEST(LinkedList, MoveConstructor)
{
    linked_list<i32> orig = {1, 2, 3};
    linked_list<i32> copy = helios::move(orig);
    auto copyIt = copy.begin();

    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(orig.size(), 0);
    EXPECT_EQ(*copyIt, 1);
    ++copyIt;
    EXPECT_EQ(*copyIt, 2);
    ++copyIt;
    EXPECT_EQ(*copyIt, 3);
}

TEST(LinkedList, InitializerListAssignment)
{
    linked_list<i32> orig = {1, 2, 3};
    orig = {4, 5, 6, 7};
    auto it = orig.begin();

    EXPECT_EQ(orig.size(), 4);
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 7);
}

TEST(LinkedList, CopyAssignment)
{
    linked_list<i32> orig = {1, 2, 3};
    linked_list<i32> copy = {4, 5, 6, 7};
    copy = orig;
    auto origIt = orig.begin();
    auto copyIt = copy.begin();

    EXPECT_EQ(orig.size(), copy.size());
    EXPECT_EQ(*origIt, *copyIt);
    ++origIt;
    ++copyIt;
    EXPECT_EQ(*origIt, *copyIt);
    ++origIt;
    ++copyIt;
    EXPECT_EQ(*origIt, *copyIt);
}

TEST(LinkedList, MoveAssignment)
{
    linked_list<i32> orig = {1, 2, 3};
    linked_list<i32> copy = {4, 5, 6, 7};
    copy = helios::move(orig);
    auto copyIt = copy.begin();

    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(orig.size(), 0);
    EXPECT_EQ(*copyIt, 1);
    ++copyIt;
    EXPECT_EQ(*copyIt, 2);
    ++copyIt;
    EXPECT_EQ(*copyIt, 3);
}

TEST(LinkedList, EqualityOperator)
{
    linked_list<i32> lhs = {1, 2, 3};
    linked_list<i32> rhs = {1, 2, 3};
    EXPECT_EQ(lhs, rhs);
    EXPECT_TRUE(lhs == rhs);

    linked_list<i32> ne = {4, 5, 6, 7};
    EXPECT_FALSE(lhs == ne);
}

TEST(LinkedList, InequalityOperator)
{
    linked_list<i32> lhs = {1, 2, 3};
    linked_list<i32> rhs = {1, 2, 3};
    linked_list<i32> ne = {4, 5, 6, 7};
    EXPECT_NE(lhs, ne);
    EXPECT_TRUE(lhs != ne);

    EXPECT_FALSE(lhs == ne);
}

TEST(LinkedList, Front)
{
    linked_list<i32> li = {1, 2, 3};
    auto f = li.front();

    EXPECT_EQ(f, 1);
}

TEST(LinkedList, Back)
{
    linked_list<i32> li = {1, 2, 3};
    auto b = li.back();

    EXPECT_EQ(b, 3);
}

TEST(LinkedList, Begin)
{
    linked_list<i32> li = {1, 2, 3};
    auto f = li.begin();

    EXPECT_EQ(*f, 1);
}

TEST(LinkedList, End)
{
    linked_list<i32> li = {1, 2, 3};
    auto b = li.end();

    EXPECT_EQ(*(--b), 3);
}

TEST(LinkedList, EmplaceFront)
{
    struct Test
    {
        int x, y;
        Test(int x, int y) : x(x), y(y)
        {
        }
    };

    linked_list<Test> list;
    list.emplace_front(3, 4);
    list.emplace_front(5, 6);

    EXPECT_EQ(list.size(), 2);
    EXPECT_FALSE(list.empty());
    auto it = list.begin();
    EXPECT_EQ((*it).x, 5);
    EXPECT_EQ((*it).y, 6);
    ++it;
    EXPECT_EQ((*it).x, 3);
    EXPECT_EQ((*it).y, 4);
}

TEST(LinkedList, PushFront)
{
    linked_list<i32> li;
    li.push_front(2);
    li.push_front(1);

    EXPECT_EQ(li.size(), 2);
    EXPECT_FALSE(li.empty());
    auto it = li.begin();

    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(LinkedList, EmplaceBack)
{
    struct Test
    {
        int x, y;
        Test(int x, int y) : x(x), y(y)
        {
        }
    };

    linked_list<Test> list;
    list.emplace_back(5, 6);
    list.emplace_back(3, 4);

    EXPECT_EQ(list.size(), 2);
    EXPECT_FALSE(list.empty());
    auto it = list.begin();
    EXPECT_EQ((*it).x, 5);
    EXPECT_EQ((*it).y, 6);
    ++it;
    EXPECT_EQ((*it).x, 3);
    EXPECT_EQ((*it).y, 4);
}

TEST(LinkedList, PushBack)
{
    linked_list<i32> li;
    li.push_back(1);
    li.push_back(2);

    EXPECT_EQ(li.size(), 2);
    EXPECT_FALSE(li.empty());
    auto it = li.begin();

    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(LinkedList, Emplace)
{
    struct Test
    {
        int x, y;
        Test(int x, int y) : x(x), y(y)
        {
        }
    };

    linked_list<Test> list = {{1, 2}, {5, 6}};
    auto it = ++list.begin();
    it = list.emplace(it, 3, 4);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ((*it).x, 3);
    EXPECT_EQ((*it).y, 4);

    it = list.begin();
    EXPECT_EQ((*it).x, 1);
    EXPECT_EQ((*it).y, 2);
    ++it;
    EXPECT_EQ((*it).x, 3);
    EXPECT_EQ((*it).y, 4);
    ++it;
    EXPECT_EQ((*it).x, 5);
    EXPECT_EQ((*it).y, 6);
    ++it;
    EXPECT_EQ(it, list.end());
}

TEST(LinkedList, Insert)
{
    linked_list<i32> li = {1, 3};
    auto it = ++li.begin();
    it = li.insert(it, 2);
    EXPECT_EQ(li.size(), 3);
    EXPECT_EQ(*it, 2);
    it = li.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, li.end());
}

TEST(LinkedList, PopFront)
{
    linked_list<i32> li = {1, 2, 3};
    EXPECT_EQ(li.size(), 3);
    li.pop_front();
    EXPECT_EQ(li.size(), 2);
    EXPECT_EQ(li.front(), 2);
}

TEST(LinkedList, PopBack)
{
    linked_list<i32> li = {1, 2, 3};
    EXPECT_EQ(li.size(), 3);
    li.pop_back();
    EXPECT_EQ(li.size(), 2);
    EXPECT_EQ(li.back(), 2);
}

TEST(LinkedList, Erase)
{
    linked_list<i32> li = {1, 2, 3};
    EXPECT_EQ(li.size(), 3);
    auto it = li.begin();
    ++it;
    li.erase(it);
    EXPECT_EQ(li.size(), 2);
    EXPECT_EQ(li.front(), 1);
    EXPECT_EQ(li.back(), 3);
    it = li.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, li.end());
}

TEST(LinkedList, EraseRange)
{
    linked_list<i32> li = {1, 2, 3, 4};
    EXPECT_EQ(li.size(), 4);
    auto it = li.begin();
    auto start = ++it;
    ++it;
    auto end = ++it;
    li.erase(start, end);
    EXPECT_EQ(li.size(), 2);
    EXPECT_EQ(li.front(), 1);
    EXPECT_EQ(li.back(), 4);
}

TEST(LinkedList, Clear)
{
    linked_list<i32> li = {1, 2, 3, 4};
    EXPECT_EQ(li.size(), 4);
    EXPECT_NE(li.begin(), li.end());
    li.clear();
    EXPECT_EQ(li.size(), 0);
    EXPECT_TRUE(li.empty());
    EXPECT_EQ(li.begin(), li.end());
}
