#include <helios/ecs/entity_manager.hpp>

#include <gtest/gtest.h>

using namespace helios;

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