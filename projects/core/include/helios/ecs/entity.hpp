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

        template <typename Component>
        Component& get();

        template <typename Component>
        const Component& get() const;

        template <typename Component, typename... Args>
        Component& assign(Args&&... args);

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

        template <typename Func, typename ... Components>
        void each(Func fn);

    private:
        friend entt::registry& get_entt(EntityManager& manager);

        entt::registry _registry;
    };

    template <typename Component>
    inline Component& Entity::get()
    {
        return _manager->_registry.get<Component>(_ent);
    }

    template <typename Component>
    inline const Component& Entity::get() const
    {
        return _manager->_registry.get<Component>(_ent);
    }

    template <typename Component, typename... Args>
    inline Component& Entity::assign(Args&&... args)
    {
        return _manager->_registry.emplace<Component>(
            _ent, helios::forward<Args>(args)...);
    }

    template <typename Component, typename... Args>
    inline Component& Entity::replace(Args&&... args)
    {
        return _manager->_registry.replace<Component>(
            _ent, helios::forward<Args>(args)...);
    }

    template <typename... Components>
    inline bool Entity::has() const
    {
        return _manager->_registry.has<Components...>(_ent);
    }

    template <typename Component>
    inline void Entity::remove()
    {
        return _manager->_registry.remove<Component>(_ent);
    }
    
    template <typename Func, typename... Components>
    inline void EntityManager::each(Func fn)
    {
        _registry.view<Components...>().each(fn);
    }
} // namespace helios
