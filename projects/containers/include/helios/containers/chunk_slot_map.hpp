#pragma once

#include <helios/containers/memory.hpp>
#include <helios/containers/pool.hpp>

namespace helios
{
    template <typename Value, size_t ElementsPerChunk, typename Allocator>
    class chunk_slot_map;

    template <typename Value, size_t ElementsPerChunk, typename Allocator>
    class chunk_slot_map_iterator
    {
        friend class chunk_slot_map<Value, ElementsPerChunk, Allocator>;

        chunk_slot_map_iterator(
            const chunk_slot_map<Value, ElementsPerChunk, Allocator>* map,
            u32 index)
            : _map(const_cast<
                   chunk_slot_map<Value, ElementsPerChunk, Allocator>*>(map)),
              _index(index)
        {
        }

    public:
        chunk_slot_map_iterator(const chunk_slot_map_iterator&) = default;
        chunk_slot_map_iterator(chunk_slot_map_iterator&&) noexcept = default;
        ~chunk_slot_map_iterator() = default;
        chunk_slot_map_iterator& operator=(const chunk_slot_map_iterator&) =
            default;
        chunk_slot_map_iterator& operator=(chunk_slot_map_iterator&&) noexcept =
            default;

        bool operator==(const chunk_slot_map_iterator& rhs) const noexcept
        {
            return _index == rhs._index && _map == rhs._map;
        }

        bool operator!=(const chunk_slot_map_iterator& rhs) const noexcept
        {
            return _index != rhs._index || _map != rhs._map;
        }

        chunk_slot_map_iterator& operator++()
        {
            ++_index;
            return *this;
        }

        chunk_slot_map_iterator operator++(int)
        {
            auto cpy = *this;
            ++_index;
            return cpy;
        }

        chunk_slot_map_iterator& operator--()
        {
            --_index;
            return *this;
        }

        chunk_slot_map_iterator operator--(int)
        {
            auto cpy = *this;
            --_index;
            return cpy;
        }

        Value& operator*()
        {
            return _map->_values.at(_index);
        }

        const Value& operator*() const
        {
            return _map->_values.at(_index);
        }

    private:
        u32 _index;
        chunk_slot_map<Value, ElementsPerChunk, Allocator>* _map;
    };

    class chunk_slot_key
    {
        template <typename Value, size_t ElementsPerChunk, typename Allocator>
        friend class chunk_slot_map;

        chunk_slot_key(u32 index, u32 generation);

    public:
        chunk_slot_key(const chunk_slot_key&) = default;
        chunk_slot_key(chunk_slot_key&&) noexcept = default;
        ~chunk_slot_key() = default;
        chunk_slot_key& operator=(const chunk_slot_key&) = default;
        chunk_slot_key& operator=(chunk_slot_key&&) noexcept = default;

    private:
        u32 _index;
        u32 _generation;
    };

    template <typename Value, size_t ElementsPerChunk,
              typename Allocator = allocator<Value>>
    class chunk_slot_map
    {
        friend class chunk_slot_map_iterator<Value, ElementsPerChunk,
                                             Allocator>;

        struct slot_index
        {
            u32 generation;
            union
            {
                u32 index;
                u32 next;
            };
        };

    public:
        chunk_slot_map() : _free_head(~0U), _count(0), _chunkCount(0)
        {
        }

        chunk_slot_map(const chunk_slot_map& other)
            : _free_head(other._free_head), _count(other._count),
              _chunkCount(other._chunkCount)
        {
            _indices = other._indices;
            _values = other._values;
            _erase = other._erase;
        }

        chunk_slot_map(chunk_slot_map&& other) noexcept
            : _free_head(other._free_head), _count(other._count),
              _chunkCount(other._chunkCount)
        {
            _indices = helios::move(other._indices);
            _values = helios::move(other._values);
            _erase = helios::move(other._erase);

            other._count = 0;
        }

        ~chunk_slot_map()
        {
            for (u32 i = 0; i < _count; i++)
            {
                _values.at(i).~Value();
            }
        }

        chunk_slot_map& operator=(const chunk_slot_map& other)
        {
            clear();

            _free_head = other._free_head;
            _count = other._count;
            _chunkCount = other._chunkCount;

            _indices = other._indices;
            _values = other._values;
            _erase = other._erase;

            return *this;
        }

        chunk_slot_map& operator=(chunk_slot_map&& other) noexcept
        {
            clear();

            _free_head = other._free_head;
            _count = other._count;
            _chunkCount = other._chunkCount;

            _indices = helios::move(other._indices);
            _values = helios::move(other._values);
            _erase = helios::move(other._erase);

            other._count = 0;

            return *this;
        }

        chunk_slot_map_iterator<Value, ElementsPerChunk, Allocator> begin()
            const noexcept
        {
            return chunk_slot_map_iterator<Value, ElementsPerChunk, Allocator>(
                this, 0);
        }

        chunk_slot_map_iterator<Value, ElementsPerChunk, Allocator> end() const
            noexcept
        {
            return chunk_slot_map_iterator<Value, ElementsPerChunk, Allocator>(
                this, _count);
        }

        size_t capacity() const noexcept
        {
            return _values.capacity();
        }

        bool empty() const noexcept
        {
            return _count == 0;
        }

        size_t size() const noexcept
        {
            return _count;
        }

        bool contains(const chunk_slot_key& key)
        {
            if (key._index < _count)
            {
                auto& idx = _indices.at(key._index);
                return idx.generation == key._generation;
            }
            return false;
        }

        Value& get(const chunk_slot_key& key)
        {
            if (key._index < _count)
            {
                auto& idx = _indices.at(key._index);
                if (idx.generation == key._generation)
                {
                    auto index = idx.index;
                    return _values.at(index);
                }
            }
            return *(reinterpret_cast<Value*>(nullptr));
        }

        const Value& get(const chunk_slot_key& key) const
        {
            if (key._index < _count)
            {
                auto& idx = _indices.at(key._index);
                if (idx.generation == key._generation)
                {
                    auto index = idx.index;
                    return _values.at(index);
                }
            }
            return *(reinterpret_cast<Value*>(nullptr));
        }

        Value* try_get(const chunk_slot_key& key) noexcept
        {
            auto& idx = _indices.at(key._index);
            if (idx.generation == key._generation)
            {
                auto index = idx.index;
                return &_values.at(index);
            }
            return nullptr;
        }

        const Value* try_get(const chunk_slot_key& key) const noexcept
        {
            auto& idx = _indices.at(key._index);
            if (idx.generation == key._generation)
            {
                auto index = idx.index;
                return &_values.at(index);
            }
            return nullptr;
        }

        void clear()
        {
            // reset everything
            for (u32 i = 0; i < _count; i++)
            {
                _values.at(i).~Value();
                _indices.at(i).generation++;
            }

            // set the free chain
            if (capacity() > 0)
            {
                for (u32 i = 0; i < capacity() - 1; i++)
                {
                    _indices.at(i).next = i + 1;
                }
                _indices.at(capacity() - 1).next = ~0U;
            }

            _count = 0;
        }

        bool erase(const chunk_slot_key& key)
        {
            if (key._index < _count)
            {
                auto& index = _indices.at(key._index);
                if (index.generation == key._generation)
                {
                    u32 val = index.index;
                    _values.at(val).~Value();

                    _values.set(val, _values.at(_count - 1));
                    _values.at(_count - 1).~Value();

                    _erase.set(val, _erase.at(_count - 1));

                    _indices.at(_erase.at(val)).index = val;
                    index.generation += 1;
                    index.next = _free_head;
                    _free_head = key._index;
                    --_count;
                    return true;
                }
            }
            return false;
        }

        chunk_slot_key insert(const Value& value)
        {
            if (_free_head == ~0U)
            {
                _resize();
            }

            u32 free = _free_head;
            auto& idx = _indices.at(free);
            _free_head = idx.next;

            idx.index = _count;
            _values.set(_count, value);
            _erase.set(_count, free);
            _count++;

            return chunk_slot_key(free, idx.generation);
        }

        chunk_slot_key insert(Value&& value)
        {
            if (_free_head == ~0U)
            {
                _resize();
            }

            u32 free = _free_head;
            auto& idx = _indices.at(free);
            _free_head = idx.next;

            idx.index = _count;
            _values.set(_count, helios::move(value));
            _erase.set(_count, free);
            _count++;

            return chunk_slot_key(free, idx.generation);
        }

    private:
        memory_pool<slot_index, ElementsPerChunk> _indices;
        memory_pool<Value, ElementsPerChunk> _values;
        memory_pool<u32, ElementsPerChunk> _erase;

        u32 _free_head;

        size_t _count;

        Allocator _alloc;
        u32 _chunkCount;

        void _resize()
        {
            _indices.cleared_resize(_chunkCount + 1);
            _values.resize(_chunkCount + 1);
            _erase.resize(_chunkCount + 1);

            _free_head = _chunkCount * ElementsPerChunk;
            for (u32 i = _free_head; i < _free_head + ElementsPerChunk - 1; i++)
            {
                _indices.at(i).next = i + 1;
            }
            _indices.at(_free_head + ElementsPerChunk - 1).next = ~0U;

            _chunkCount += 1;
        }
    };
} // namespace helios