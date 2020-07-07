#include <helios/ecs/entity_manager.hpp>

#include <gtest/gtest.h>

using namespace helios;

struct DummyType
{
    u32 value;

    DummyType(u32 value) : value(value)
    {
    }
};

struct OtherDummyType
{
    f32 value;

    OtherDummyType(f32 value) : value(value)
    {
    }
};

TEST(EntityManager, DefaultConstructor)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());
}

TEST(EntityManager, Allocate)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());

    Entity e1 = manager.allocate();
    ASSERT_EQ(1, manager.getEntityCount());

    Entity e2 = manager.allocate();
    ASSERT_EQ(2, manager.getEntityCount());
}

TEST(EntityManager, Assign)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());

    Entity e1 = manager.allocate();
    ASSERT_EQ(1, manager.getEntityCount());

    auto comp = manager.assign<DummyType>(e1, 2U);

    ASSERT_TRUE(manager.contains<DummyType>(e1));
    ASSERT_FALSE(manager.contains<OtherDummyType>(e1));
    ASSERT_EQ(comp->value, 2U);
    ASSERT_EQ((*comp).value, 2U);
}

TEST(EntityManager, Remove)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());

    Entity e1 = manager.allocate();
    ASSERT_EQ(1, manager.getEntityCount());

    manager.assign<DummyType>(e1, 2U);

    ASSERT_TRUE(manager.contains<DummyType>(e1));
    ASSERT_FALSE(manager.contains<OtherDummyType>(e1));
    ASSERT_TRUE(manager.remove<DummyType>(e1));
    ASSERT_FALSE(manager.contains<DummyType>(e1));
}

TEST(EntityManager, Get)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());

    Entity e1 = manager.allocate();
    ASSERT_EQ(1, manager.getEntityCount());

    manager.assign<DummyType>(e1, 2U);

    ASSERT_TRUE(manager.contains<DummyType>(e1));
    ASSERT_FALSE(manager.contains<OtherDummyType>(e1));

    auto& comp = manager.get<DummyType>(e1);
    ASSERT_EQ(comp.value, 2U);
}

TEST(EntityManager, RemovedHandle)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());

    Entity e1 = manager.allocate();
    ASSERT_EQ(1, manager.getEntityCount());

    auto comp = manager.assign<DummyType>(e1, 2U);
    ASSERT_TRUE(comp);
    ASSERT_EQ(comp->value, 2U);

    ASSERT_TRUE(manager.contains<DummyType>(e1));
    ASSERT_FALSE(manager.contains<OtherDummyType>(e1));
    ASSERT_TRUE(manager.remove<DummyType>(e1));
    ASSERT_FALSE(manager.contains<DummyType>(e1));

    ASSERT_FALSE(comp);
}
