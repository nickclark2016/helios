#pragma once

#include <helios/containers/memory.hpp>
#include <helios/containers/vector.hpp>

namespace helios
{
    template <typename Type, size_t Elements,
              typename Allocator = allocator<Type>>
    class memory_pool
    {
        static constexpr bool is_pow_2(u64 v)
        {
            return v && ((v & (v - 1)) == 0);
        }

        static constexpr u64 mod_2(u64 n, u64 d)
        {
            return n & (d - 1);
        }

        static constexpr u64 log_2(u64 n)
        {
            return n == 1 ? 0 : 1 + log_2(n >> 1);
        }

    public:
        memory_pool() : _cursor(0), _capacity(0)
        {
            static_assert(is_pow_2(Elements));
        }

        memory_pool(const memory_pool& other)
            : _cursor(other._cursor), _capacity(other._capacity)
        {
            static_assert(is_pow_2(Elements));
            resize(other._data.capacity());
            _alloc = other._alloc;
            for (u64 i = 0; i < other._cursor; i++)
            {
                set(i, other.at(i));
            }
        }

        memory_pool(memory_pool&& other) noexcept
            : _cursor(other._cursor), _capacity(other._capacity),
              _data(helios::move(other._data))
        {
            static_assert(is_pow_2(Elements));
            _alloc = helios::move(other._alloc);
        }

        ~memory_pool()
        {
            static_assert(is_pow_2(Elements));
            clear();
        }

        memory_pool& operator=(const memory_pool& rhs)
        {
            clear();

            resize(rhs._data.capacity());

            _cursor = rhs._cursor;
            _capacity = rhs._capacity;

            _alloc = rhs._alloc;
            for (u64 i = 0; i < rhs._cursor; i++)
            {
                set(i, rhs.at(i));
            }

            return *this;
        }

        memory_pool& operator=(memory_pool&& rhs) noexcept
        {
            clear();

            _cursor = rhs._cursor;
            _capacity = rhs._capacity;
            _data = helios::move(rhs._data);

            return *this;
        }

        u64 capacity() const noexcept
        {
            return _capacity;
        }

        bool empty() const noexcept
        {
            return _cursor == 0;
        }

        u64 size() const noexcept
        {
            return _cursor;
        }

        Type& at(u64 index)
        {
            u64 idx = index >> shift;
            u64 off = mod_2(index, Elements);
            return _data[idx][off];
        }

        const Type& at(u64 index) const
        {
            u64 idx = index >> shift;
            u64 off = mod_2(index, Elements);
            return _data[idx][off];
        }

        Type& set(u64 index, const Type& value)
        {
            _cursor = index + 1 > _cursor ? index + 1 : _cursor;
            u64 idx = index >> shift;
            u64 off = mod_2(index, Elements);
            return *(::new (_data[idx] + off) Type(value));
        }

        Type& set(u64 index, Type&& value)
        {
            _cursor = index + 1 > _cursor ? index + 1 : _cursor;
            u64 idx = index >> shift;
            u64 off = mod_2(index, Elements);
            return *(::new (_data[idx] + off) Type(helios::move(value)));
        }

        void clear()
        {
            for (const auto& buf : _data)
            {
                _alloc.release(buf);
            }
            _data.clear();
        }

        void resize(u64 chunks)
        {
            _data.reserve(chunks);
            for (u64 i = _data.size(); _data.size() < chunks; i++)
            {
                Type* value = _alloc.allocate(Elements);
                _data.push_back(value);
            }
            if (chunks * Elements > _capacity)
            {
                _capacity = chunks * Elements;
            }
        }

        void cleared_resize(u64 chunks)
        {
            _data.reserve(chunks);
            for (u64 i = _data.size(); _data.size() < chunks; i++)
            {
                Type* value = _alloc.allocate(Elements);
                memset(value, 0, sizeof(Type) * Elements);
                _data.push_back(value);
            }
            if (chunks * Elements > _capacity)
            {
                _capacity = chunks * Elements;
            }
        }

    private:
        vector<Type*> _data;
        u64 _cursor;
        u64 _capacity;
        Allocator _alloc;

        static constexpr u64 shift = log_2(Elements);
    };
} // namespace helios
