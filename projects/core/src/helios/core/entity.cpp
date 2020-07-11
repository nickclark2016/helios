#include <helios/core/entity.hpp>

#include <entt/entt.hpp>

namespace helios
{
    Entity::Entity(entt::entity e, EntityManager* manager)
        : _ent(e), _manager(manager)
    {
    }

    Entity EntityManager::create()
    {
        return Entity(_registry.create(), this);
    }

    void EntityManager::release(Entity ent)
    {
        _registry.destroy(ent._ent);
    }

    void EntityManager::releaseAll()
    {
        _registry.clear();
    }
} // namespace helios
