#pragma once

#include <helios/containers/chunk_slot_map.hpp>
#include <helios/containers/slot_map.hpp>
#include <helios/ecs/entity.hpp>
#include <helios/macros.hpp>

namespace helios
{
    template <u64 MaxComponentTypes = 1024,
              u64 MaxComponentsPerType = 1024 * 1024,
              u64 EntitiesPerChunk = 4096>
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
            auto it = _entities.insert({});
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
        };

        struct IPool
        {
        };

        template <typename T>
        struct FixedPool : IPool
        {
            slot_map<T> slots;
        };

        chunk_slot_map<EntityInternal, EntitiesPerChunk> _entities;
        IPool* _pools[MaxComponentTypes];
    };
} // namespace helios
