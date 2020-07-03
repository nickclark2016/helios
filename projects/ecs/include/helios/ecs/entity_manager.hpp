#pragma once

#include <helios/containers/chunk_slot_map.hpp>
#include <helios/containers/memory.hpp>
#include <helios/containers/slot_map.hpp>
#include <helios/ecs/entity.hpp>
#include <helios/macros.hpp>

namespace helios
{
    template <u32 MaxComponentTypes = 1024,
              u32 MaxComponentsPerType = 1024 * 1024,
              u32 EntitiesPerChunk = 4096>
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
        struct ComponentMapKV
        {
            u16 idx;
            slot_key key;
        };

        struct ComponentMap
        {
            ComponentMap(block_allocator<ComponentMapKV, MaxComponentsPerType,
                                         EMemoryTag::ECS_COMPONENT_MAP>& alloc)
                : allocator(alloc)
            {
            }

            ~ComponentMap()
            {
                allocator.release_all();
            }

            block_allocator<ComponentMapKV, MaxComponentsPerType,
                            EMemoryTag::ECS_COMPONENT_MAP>& allocator;
            u16* keys;
            slot_key* values;
            u32 count;
            u32 capacity;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
            slot_key* get(u16 key)
            {
            }

            void put(u16 key, slot_key value)
            {
            }

            void remove(u16 key)
            {
            }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        };

        struct EntityInternal
        {
            // Map of components
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

        block_allocator<ComponentMapKV, MaxComponentsPerType,
                        EMemoryTag::ECS_COMPONENT_MAP>
            _componentSlotAllocator;
        chunk_slot_map<EntityInternal, EntitiesPerChunk> _entities;
        IPool* _pools[MaxComponentTypes];
    };
} // namespace helios
