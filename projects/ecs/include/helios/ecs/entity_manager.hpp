#pragma once

#include <helios/containers/chunk_slot_map.hpp>
#include <helios/containers/memory.hpp>
#include <helios/containers/robin_hood.hpp>
#include <helios/containers/slot_map.hpp>
#include <helios/ecs/component.hpp>
#include <helios/ecs/entity.hpp>
#include <helios/macros.hpp>

namespace helios
{
    template <u32 MaxComponentTypes, u32 MaxComponentsPerType, u32 EntitiesPerChunk>
    class EntityManager;

    template <typename T, u32 MaxComponentTypes, u32 MaxComponentsPerType, u32 EntitiesPerChunk>
    class ComponentHandle
    {
        friend class EntityManager<MaxComponentTypes, MaxComponentsPerType, EntitiesPerChunk>;

        EntityManager<MaxComponentTypes, MaxComponentsPerType, EntitiesPerChunk>* _manager;
        Entity _owner;

        ComponentHandle(EntityManager<MaxComponentTypes, MaxComponentsPerType, EntitiesPerChunk>* manager, Entity owner)
            : _manager(manager), _owner(owner)
        {
        }

    public:
        ComponentHandle(const ComponentHandle&) = default;
        ComponentHandle(ComponentHandle&&) noexcept = default;
        ~ComponentHandle() = default;
        ComponentHandle& operator=(const ComponentHandle&) = default;
        ComponentHandle& operator=(ComponentHandle&&) noexcept = default;

        T* operator->()
        {
            return &_manager->template get<T>(_owner);
        }

        const T* operator->() const
        {
            return &_manager->template get<T>(_owner);
        }

        T& operator*()
        {
            return _manager->template get<T>(_owner);
        }

        const T& operator*() const
        {
            return _manager->template get<T>(_owner);
        }

        explicit operator bool() const
        {
            return _manager->template contains<T>(_owner);
        }
    };

    template <u32 MaxComponentTypes = 1024, u32 MaxComponentsPerType = 1024 * 1024, u32 EntitiesPerChunk = 4096>
    class EntityManager
    {
    public:
        EntityManager()
        {
            memset(_pools, 0, sizeof(IPool*) * MaxComponentTypes);
        }

        ~EntityManager()
        {
            for (auto pool : _pools)
            {
                if (pool)
                {
                    delete pool;
                }
            }

            _entities.clear();
        }

        HELIOS_NO_COPY_MOVE(EntityManager);

        Entity allocate()
        {
            EntityInternal internal;
            auto it = _entities.insert(internal);
            Entity e = {it};
            return e;
        }

        bool release(Entity& ent)
        {
            auto it = ent.uuid;
            return _entities.erase(it);
        }

        u64 getEntityCount() const noexcept
        {
            return _entities.size();
        }

        template <typename Component, typename... Args>
        ComponentHandle<Component, MaxComponentTypes, MaxComponentsPerType, EntitiesPerChunk> assign(Entity ent,
                                                                                                     Args&&... args)
        {
            static u32 id = GetComponentID<Component>();
            FixedPool<Component>* pool = reinterpret_cast<FixedPool<Component>*>(_pools[id]);
            if (pool == nullptr)
            {
                pool = new FixedPool<Component>();
                pool->slots.reserve(MaxComponentsPerType);
                _pools[id] = pool;
            }
            auto key = pool->slots.insert(Component(helios::forward<Args>(args)...));
            _entities.get(ent.uuid).components.insert({id, key});

            ComponentHandle<Component, MaxComponentTypes, MaxComponentsPerType, EntitiesPerChunk> handle(this, ent);

            return handle;
        };

        template <typename Component>
        bool contains(Entity ent) const
        {
            static u32 id = GetComponentID<Component>();
            auto& map = _entities.get(ent.uuid).components;
            return map.find(id) != map.end();
        }

        template <typename Component>
        Component& get(Entity ent)
        {
            static u32 id = GetComponentID<Component>();
            Component* comp = nullptr;
            auto& map = _entities.get(ent.uuid).components;
            auto it = map.find(id);
            if (it != map.end())
            {
                FixedPool<Component>* pool = reinterpret_cast<FixedPool<Component>*>(_pools[id]);
                comp = &pool->slots.get(it->second);
            }
            return *comp;
        }

        template <typename Component>
        bool remove(Entity ent)
        {
            static u32 id = GetComponentID<Component>();
            auto& map = _entities.get(ent.uuid).components;
            auto it = map.find(id);
            if (it != map.end())
            {
                FixedPool<Component>* pool = reinterpret_cast<FixedPool<Component>*>(_pools[id]);
                const auto result = pool->slots.erase(it->second);
                map.erase(it);
                return result;
            }
            return false;
        }

    private:
        struct EntityInternal
        {
            unordered_flat_map<u16, slot_key> components;
        };

        struct IPool
        {
            virtual ~IPool() = default;
        };

        template <typename T>
        struct FixedPool : IPool
        {
            ~FixedPool() override = default;
            slot_map<T> slots;
        };

        chunk_slot_map<EntityInternal, EntitiesPerChunk> _entities;
        IPool* _pools[MaxComponentTypes];
    };
} // namespace helios
