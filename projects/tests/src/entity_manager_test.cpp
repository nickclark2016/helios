#include <helios/ecs/entity_manager.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(EntityManager, DefaultConstructor)
{
    EntityManager manager;
    ASSERT_EQ(0, manager.getEntityCount());
}