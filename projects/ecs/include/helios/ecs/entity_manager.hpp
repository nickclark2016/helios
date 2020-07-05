#pragma once

#include <helios/containers/chunk_slot_map.hpp>
#include <helios/containers/memory.hpp>
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
        struct ComponentMapKV
        {
            struct Key
            {
                u16 idx;
                u16 next;
            };
            slot_key value;
        };

        struct ComponentMap
        {
            ComponentMap(block_allocator<ComponentMapKV, MaxComponentsPerType, EMemoryTag::ECS_COMPONENT_MAP>& alloc)
                : allocator(alloc), keys(nullptr), values(nullptr), count(0), capacity(0)
            {
            }

            ~ComponentMap()
            {
                allocator.release_all();
            }

            block_allocator<ComponentMapKV, MaxComponentsPerType, EMemoryTag::ECS_COMPONENT_MAP>& allocator;
            typename ComponentMapKV::Key* keys;
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

            bool put(u16 key, slot_key value)
            {
                if (count == capacity)
                {
                    const u32 requested = count + 8;
                    u8* allocation = reinterpret_cast<u8*>(allocator.allocate(requested));
                    if (allocation)
                    {
                        typename ComponentMapKV::Key* keyPtr =
                            reinterpret_cast<typename ComponentMapKV::Key*>(allocation);
                        memcpy(keyPtr, keys, count * sizeof(typename ComponentMapKV::Key));
                        slot_key* valuePtr =
                            reinterpret_cast<slot_key*>(allocation + (count + sizeof(typename ComponentMapKV::Key)));
                        memcpy(valuePtr, values, count * sizeof(slot_key));

                        // keys points to the start of the block
                        allocator.release(reinterpret_cast<ComponentMapKV*>(keys));

                        keys = keyPtr;
                        values = valuePtr;
                    }
                }
            }

            bool remove(u16 key)
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

        block_allocator<ComponentMapKV, MaxComponentsPerType, EMemoryTag::ECS_COMPONENT_MAP> _componentSlotAllocator;
        chunk_slot_map<EntityInternal, EntitiesPerChunk> _entities;
        IPool* _pools[MaxComponentTypes];
    };
} // namespace helios
