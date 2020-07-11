#pragma once

#include <helios/containers/utility.hpp>

#include <entt/entt.hpp>

namespace helios
{
    class Entity;
    class EntityManager;

    class Entity
    {
        friend class EntityManager;

        Entity(entt::entity e, EntityManager* manager);

    public:
        ~Entity() = default;

        template <typename Component, typename... Args>
        Component& emplace(Args&&... args);

        template <typename... Components>
        bool has() const;

        template <typename Component>
        void remove();

        template <typename Component, typename... Args>
        Component& replace(Args&&... args);

    private:
        entt::entity _ent;
        EntityManager* _manager;
    };

    class EntityManager
    {
        friend class Entity;

    public:
        Entity create();
        void release(Entity ent);
        void releaseAll();

    private:
        entt::registry _registry;
    };

    template <typename Component, typename... Args>
    Component& Entity::emplace(Args&&... args)
    {
        return _manager->_registry.emplace<Component>(
            _ent, helios::forward<Args>(args)...);
    }

    template <typename Component, typename... Args>
    Component& Entity::replace(Args&&... args)
    {
        return _manager->_registry.replace<Component>(
            _ent, helios::forward<Args>(args)...);
    }

    template <typename... Components>
    bool Entity::has() const
    {
        return _manager->_registry.has<Components...>(_ent);
    }

    template <typename Component>
    void Entity::remove()
    {
        return _manager->_registry.remove<Component>(_ent);
    }
} // namespace helios
