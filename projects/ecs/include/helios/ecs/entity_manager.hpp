#pragma once

#include <helios/containers/chunk_slot_map.hpp>
#include <helios/containers/memory.hpp>
#include <helios/containers/robin_hood.hpp>
#include <helios/containers/slot_map.hpp>
#include <helios/ecs/entity.hpp>
#include <helios/macros.hpp>

namespace helios
{
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
            e.uuid = it;
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
