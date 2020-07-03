#pragma once

#include <helios/containers/memory.hpp>

namespace helios
{
    template <typename Value, typename Allocator>
    class slot_map;

    class slot_key
    {
        template <typename K, typename A>
        friend class slot_map;

    public:
        slot_key();
        slot_key(u32 index, u32 generation);
        slot_key(const slot_key&) = default;
        slot_key(slot_key&&) noexcept = default;
        ~slot_key() = default;
        slot_key& operator=(const slot_key&) = default;
        slot_key& operator=(slot_key&&) noexcept = default;

    private:
        u32 _index;
        u32 _generation;
    };

    template <typename Value, typename Allocator = allocator<Value>>
    class slot_map
    {
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
        slot_map();
        slot_map(const slot_map& other);
        slot_map(slot_map&& other) noexcept;
        ~slot_map();

        slot_map& operator=(const slot_map& other);
        slot_map& operator=(slot_map&& other) noexcept;

        Value* begin() const noexcept;
        Value* end() const noexcept;

        size_t capacity() const noexcept;
        bool empty() const noexcept;
        size_t size() const noexcept;

        bool contains(const slot_key& key);
        Value& get(const slot_key& key);
        const Value& get(const slot_key& key) const;
        Value* try_get(const slot_key& key) noexcept;
        const Value* try_get(const slot_key& key) const noexcept;

        void clear();
        bool erase(const slot_key& key);

        slot_key insert(const Value& value);
        slot_key insert(Value&& value);

    private:
        slot_index* _indices;
        Value* _values;
        u32* _erase;

        u32 _free_head;

        size_t _count;
        size_t _capacity;

        Allocator _alloc;

        void _resize(const size_t capacity);
    };

    template <typename Value, typename Allocator>
    slot_map<Value, Allocator>::slot_map()
        : _count(0), _capacity(0), _indices(nullptr), _values(nullptr),
          _erase(nullptr), _free_head(~0U)
    {
        _resize(8);
    }

    template <typename Value, typename Allocator>
    slot_map<Value, Allocator>::slot_map(
        const slot_map<Value, Allocator>& other)
        : _count(other._count), _capacity(other._capacity), _indices(nullptr),
          _values(nullptr), _erase(nullptr), _free_head(~0U)
    {
        _resize(other._capacity);
        memcpy(_indices, other._indices, other._count * sizeof(slot_index));
        memcpy(_erase, other._erase, other._count * sizeof(u32));

        for (u64 i = 0; i < _count; i++)
        {
            _values[i] = other._values[i];
        }

        _alloc = other._alloc;
        _free_head = other._free_head;
    }

    template <typename Value, typename Allocator>
    slot_map<Value, Allocator>::slot_map(
        slot_map<Value, Allocator>&& other) noexcept
        : _count(0), _capacity(0), _indices(nullptr), _values(nullptr),
          _erase(nullptr), _free_head(~0U)
    {
        _indices = other._indices;
        _values = other._values;
        _erase = other._erase;
        _count = other._count;
        _capacity = other._capacity;
        _free_head = helios::move(other._free_head);

        other._indices = nullptr;
        other._values = nullptr;
        other._erase = nullptr;

        _alloc = helios::move(other._alloc);
    }

    template <typename Value, typename Allocator>
    slot_map<Value, Allocator>::~slot_map()
    {
        delete[] _indices;
        delete[] _erase;
        for (u64 i = 0; i < _count; i++)
        {
            _values[i].~Value();
        }
        _alloc.release(_values);
    }

    template <typename Value, typename Allocator>
    slot_map<Value, Allocator>& slot_map<Value, Allocator>::operator=(
        const slot_map<Value, Allocator>& other)
    {
        clear();
        _count = other._count;

        _resize(other._capacity);
        memcpy(_indices, other._indices, other._count * sizeof(slot_index));
        memcpy(_erase, other._erase, other._count * sizeof(u32));

        for (u64 i = 0; i < _count; i++)
        {
            _values[i] = other._values[i];
        }

        _alloc = other._alloc;
        _free_head = other._free_head;

        return *this;
    }

    template <typename Value, typename Allocator>
    slot_map<Value, Allocator>& slot_map<Value, Allocator>::operator=(
        slot_map<Value, Allocator>&& other) noexcept
    {
        clear();
        _count = other._count;

        _indices = other._indices;
        _values = other._values;
        _erase = other._erase;
        _count = other._count;
        _capacity = other._capacity;
        _free_head = helios::move(other._free_head);

        other._indices = nullptr;
        other._values = nullptr;
        other._erase = nullptr;

        _alloc = helios::move(other._alloc);

        return *this;
    }

    template <typename Value, typename Allocator>
    Value* slot_map<Value, Allocator>::begin() const noexcept
    {
        return _values;
    }

    template <typename Value, typename Allocator>
    size_t slot_map<Value, Allocator>::capacity() const noexcept
    {
        return _capacity;
    }

    template <typename Value, typename Allocator>
    void slot_map<Value, Allocator>::clear()
    {
        if (_count > 0)
        {
            // release all the values
            for (u64 i = 0; i < _count; i++)
            {
                _values[i].~Value();
                _indices[i].generation += 1;
            }

            // Reset the free slots
            _indices[_count - 1].next = ~0U;
            _free_head = 0;
            for (u32 i = 0; i < _count - 1; i++)
            {
                _indices[i].next = i + 1;
            }

            _count = 0;
        }
    }

    template <typename Value, typename Allocator>
    bool slot_map<Value, Allocator>::contains(const slot_key& key)
    {
        u32 idx = key._index;
        if (idx < _count)
        {
            u32 generation = key._generation;
            return _indices[idx].generation == generation;
        }
        return false;
    }

    template <typename Value, typename Allocator>
    bool slot_map<Value, Allocator>::empty() const noexcept
    {
        return _count == 0;
    }

    template <typename Value, typename Allocator>
    Value* slot_map<Value, Allocator>::end() const noexcept
    {
        return _values + _count;
    }

    template <typename Value, typename Allocator>
    bool slot_map<Value, Allocator>::erase(const slot_key& key)
    {
        u32 idx = key._index;
        if (idx < _count)
        {
            u32 generation = key._generation;
            if (_indices[idx].generation == generation)
            {
                u32 valueIndex = _indices[idx].index;
                // destruct value in place
                _values[valueIndex].~Value();

                // move the value at the end
                ::new (_values + valueIndex)
                    Value(helios::move(_values[_indices[_count - 1].index]));

                // move the erase value
                _erase[valueIndex] = _erase[_count - 1];

                // update the indices
                _indices[_erase[valueIndex]].index = valueIndex;
                _indices[idx].generation += 1;

                // update the free list
                _indices[idx].next = _free_head;
                _free_head = idx;

                --_count;

                return true;
            }
        }
        return false;
    }

    template <typename Value, typename Allocator>
    Value& slot_map<Value, Allocator>::get(const slot_key& key)
    {
        u32 index = key._index;
        if (index < _count)
        {
            u32 generation = key._generation;
            auto idx = _indices[index];
            if (idx.generation == generation)
            {
                return _values[idx.index];
            }
        }
        return *((Value*)nullptr);
    }

    template <typename Value, typename Allocator>
    const Value& slot_map<Value, Allocator>::get(const slot_key& key) const
    {
        u32 index = key._index;
        if (index < _count)
        {
            u32 generation = key._generation;
            auto idx = _indices[index];
            if (idx.generation == generation)
            {
                return _values[idx.index];
            }
        }
        return *((Value*)nullptr);
    }

    template <typename Value, typename Allocator>
    slot_key slot_map<Value, Allocator>::insert(const Value& value)
    {
        if (_free_head == ~0U)
        {
            _resize(_capacity * 2);
        }
        u32 free = _free_head;
        auto& idx = _indices[free];
        _free_head = idx.next;

        idx.index = _count;
        ::new (_values + _count) Value(value);
        _erase[_count] = free;

        _count++;

        return slot_key(free, idx.generation);
    }

    template <typename Value, typename Allocator>
    slot_key slot_map<Value, Allocator>::insert(Value&& value)
    {
        if (_free_head == ~0U)
        {
            _resize(_capacity * 2);
        }
        u32 free = _free_head;
        auto& idx = _indices[free];
        _free_head = idx.next;

        idx.index = _count;
        ::new (_values + _count) Value(helios::move(value));
        _erase[_count] = free;

        _count++;

        return slot_key(free, idx.generation);
    }

    template <typename Value, typename Allocator>
    size_t slot_map<Value, Allocator>::size() const noexcept
    {
        return _count;
    }

    template <typename Value, typename Allocator>
    Value* slot_map<Value, Allocator>::try_get(const slot_key& key) noexcept
    {
        u32 index = key._index;
        if (index < _count)
        {
            u32 generation = key._generation;
            auto idx = _indices[index];
            if (idx.generation == generation)
            {
                return _values + idx.index;
            }
        }
        return nullptr;
    }

    template <typename Value, typename Allocator>
    const Value* slot_map<Value, Allocator>::try_get(
        const slot_key& key) const noexcept
    {
        u32 index = key._index;
        if (index < _count)
        {
            u32 generation = key._generation;
            auto idx = _indices[index];
            if (idx.generation == generation)
            {
                return _values + idx.index;
            }
        }

        return nullptr;
    }

    template <typename Value, typename Allocator>
    void slot_map<Value, Allocator>::_resize(const size_t capacity)
    {
        u32* erase = new u32[capacity];
        slot_index* indices = new slot_index[capacity];
        Value* ptr = _alloc.allocate(capacity);
        memset(erase, 0, sizeof(u32) * capacity);
        memset(indices, 0, sizeof(slot_index) * capacity);

        if (_indices)
        {
            memcpy(indices, _indices, sizeof(slot_index) * _count);
            delete[] _indices;
        }
        _indices = indices;

        if (_erase)
        {
            memcpy(erase, _erase, sizeof(u32) * _count);
            delete[] _erase;
        }
        _erase = erase;

        if (_values)
        {
            for (u64 i = 0; i < _count; i++)
            {
                ptr[i] = helios::move(_values[i]);
            }
            _alloc.release(_values);
        }
        _values = ptr;

        // go to the end of the free chain
        u32 idx = _free_head;
        while (idx != ~0U)
        {
            auto tmp = _indices[idx].next;
            if (tmp != ~0U)
            {
                idx = tmp;
                continue;
            }
            break;
        }
        if (idx == ~0U)
        {
            idx = 0;
            _free_head = 0;
        }

        for (u64 i = _count; i < capacity; i++)
        {
            _indices[idx].next = i;
            idx = i;
        }
        _indices[capacity - 1].next = ~0U;

        _capacity = capacity;
    }
} // namespace helios

#include <helios/containers/chunk_slot_map.hpp>
