#pragma once

#include <helios/containers/utility.hpp>
#include <helios/macros.hpp>

#include <cstring>

namespace helios
{
    namespace detail
    {
        template <typename Key, typename Value>
        struct unordered_map_node
        {
            pair<Key, Value> kv;
            size_t probe_count = 0;
            bool occupied = false;

            void swap(unordered_map_node& other);
        };

        template <typename Key, typename Value>
        class unordered_map_forward_iterator
        {
        public:
            unordered_map_forward_iterator(unordered_map_node<Key, Value>* payload, size_t index, size_t capacity) noexcept;
            unordered_map_forward_iterator<Key, Value>& operator++() noexcept;
            unordered_map_forward_iterator<Key, Value> operator++(int) noexcept;

            bool operator!=(const unordered_map_forward_iterator<Key, Value>& it) const noexcept;
            bool operator==(const unordered_map_forward_iterator<Key, Value>& it) const noexcept;

            pair<Key, Value>& operator*() noexcept;
            const pair<Key, Value>& operator*() const noexcept;

            pair<Key, Value>* operator->() noexcept;
            const pair<Key, Value>* operator->() const noexcept;

        private:
            unordered_map_node<Key, Value>* _payload;
            size_t _cursor;
            size_t _capacity;
        };
    } // namespace detail

	template <typename Key, typename Value, typename Hash = hash<Key>>
    class unordered_map
    {
    public:
        using forward_iterator = detail::unordered_map_forward_iterator<Key, Value>;
        using const_forward_iterator = const detail::unordered_map_forward_iterator<Key, Value>;

        unordered_map();
        unordered_map(const unordered_map& other);
        unordered_map(unordered_map&& other) noexcept;
        ~unordered_map();

        unordered_map& operator=(const unordered_map& rhs);
        unordered_map& operator=(unordered_map&& rhs) noexcept;

        bool empty() const noexcept;
        size_t size() const noexcept;
        size_t capacity() const noexcept;

        void clear();
        void resize(const size_t newSize);

        forward_iterator find(const Key& k);
        const_forward_iterator find(const Key& k) const;
        Value& operator[](const Key& k);
        Value& operator[](Key&& k);

        forward_iterator insert(const pair<Key, Value>& kv);
        forward_iterator insert(pair<Key, Value>&& kv);
        size_t erase(const Key& k);

        forward_iterator begin();
        const_forward_iterator begin() const;
        forward_iterator end();
        const_forward_iterator end() const;

    private:
        static constexpr double __rebalance_factor = 0.7;

        detail::unordered_map_node<Key, Value>* _data;
        size_t _count;
        size_t _capacity;
        Hash _hasher;

        void _resize(const size_t newSize);
        i64 _insert(const pair<Key, Value>& kv, detail::unordered_map_node<Key, Value>* buffer, size_t sz);
        i64 _insert(pair<Key, Value>&& kv, detail::unordered_map_node<Key, Value>* buffer, size_t sz);
    };

    template <typename Key, typename Value>
    inline void detail::unordered_map_node<Key, Value>::swap(unordered_map_node& other)
    {
        std::swap(kv, other.kv);
        std::swap(probe_count, other.probe_count);
        std::swap(occupied, other.occupied);
    }

    template <typename Key, typename Value>
    inline detail::unordered_map_forward_iterator<Key, Value>::unordered_map_forward_iterator(
        unordered_map_node<Key, Value>* payload, size_t index, size_t capacity) noexcept
        : _payload(payload), _cursor(index), _capacity(capacity)
    {
    }

    template <typename Key, typename Value>
    inline detail::unordered_map_forward_iterator<Key, Value>& detail::unordered_map_forward_iterator<
        Key, Value>::operator++() noexcept
    {
        while (++_cursor < _capacity)
        {
            if (_payload[_cursor].occupied)
            {
                break;
            }
        }
        return *this;
    }
    
    template <typename Key, typename Value>
    inline detail::unordered_map_forward_iterator<Key, Value> detail::unordered_map_forward_iterator<Key, Value>::operator++(
        int) noexcept
    {
        unordered_map_forward_iterator<Key, Value> res = *this;
        while (++_cursor < _capacity)
        {
            if (_payload[_cursor].occupied)
            {
                break;
            }
        }
        return res;
    }
    
    template <typename Key, typename Value>
    inline bool detail::unordered_map_forward_iterator<Key, Value>::operator!=(
        const unordered_map_forward_iterator<Key, Value>& it) const noexcept
    {
        return _cursor != it._cursor || _payload != it._payload;
    }
    
    template <typename Key, typename Value>
    inline bool detail::unordered_map_forward_iterator<Key, Value>::operator==(
        const unordered_map_forward_iterator<Key, Value>& it) const noexcept
    {
        return _cursor == it._cursor && _payload == it._payload;
    }
    
    template <typename Key, typename Value>
    inline pair<Key, Value>& detail::unordered_map_forward_iterator<Key, Value>::operator*() noexcept
    {
        return _payload[_cursor];
    }
    
    template <typename Key, typename Value>
    inline const pair<Key, Value>& detail::unordered_map_forward_iterator<Key, Value>::operator*() const noexcept
    {
        return _payload[_cursor];
    }
    
    template <typename Key, typename Value>
    inline pair<Key, Value>* detail::unordered_map_forward_iterator<Key, Value>::operator->() noexcept
    {
        return &((_payload + _cursor)->kv);
    }
    
    template <typename Key, typename Value>
    inline const pair<Key, Value>* detail::unordered_map_forward_iterator<Key, Value>::operator->() const noexcept
    {
        return &((_payload + _cursor)->kv);
    }

    template <typename Key, typename Value, typename Hash>
    inline unordered_map<Key, Value, Hash>::unordered_map() : _data(nullptr), _count(0), _capacity(0)
    {
        _resize(8);
    }

    template <typename Key, typename Value, typename Hash>
    inline unordered_map<Key, Value, Hash>::unordered_map(const unordered_map<Key, Value, Hash>& map)
        : _data(nullptr), _capacity(map._capacity), _count(0)
    {
        _resize(_capacity);
        for (auto& it : map)
        {
            insert(it);
        }
    }

    template <typename Key, typename Value, typename Hash>
    inline unordered_map<Key, Value, Hash>::unordered_map(unordered_map<Key, Value, Hash>&& map) noexcept
        : _data(map._data), _capacity(map._capacity), _count(map._count)
    {
        map._data = nullptr;
        map._capacity = map._count = 0;
    }

    template <typename Key, typename Value, typename Hash>
    inline unordered_map<Key, Value, Hash>::~unordered_map()
    {
        if (_data)
        {
            clear();
            delete[] reinterpret_cast<char*>(_data);
            _data = nullptr;
        }
    }

    template <typename Key, typename Value, typename Hash>
    unordered_map<Key, Value, Hash>& unordered_map<Key, Value, Hash>::operator=(
        const unordered_map<Key, Value, Hash>& rhs)
    {
        if (_data)
        {
            clear();
            delete[] reinterpret_cast<char*>(_data);
            _data = nullptr;
        }

        resize(rhs._capacity);

        for (auto& it : rhs)
        {
            insert(it);
        }

        return *this;
    }

    template <typename Key, typename Value, typename Hash>
    unordered_map<Key, Value, Hash>& unordered_map<Key, Value, Hash>::operator=(
        unordered_map<Key, Value, Hash>&& rhs) noexcept
    {
        _data = rhs._data;
        _capacity = rhs._capacity;
        _count = rhs._count;

        rhs._data = nullptr;
        rhs._capacity = rhs._count = 0;

        return *this;
    }

    template <typename Key, typename Value, typename Hash>
    inline bool unordered_map<Key, Value, Hash>::empty() const noexcept
    {
        return _count == 0;
    }

    template <typename Key, typename Value, typename Hash>
    inline size_t unordered_map<Key, Value, Hash>::size() const noexcept
    {
        return _count;
    }

    template <typename Key, typename Value, typename Hash>
    inline size_t unordered_map<Key, Value, Hash>::capacity() const noexcept
    {
        return _capacity;
    }

    template <typename Key, typename Value, typename Hash>
    inline void unordered_map<Key, Value, Hash>::clear()
    {
        for (i64 i = 0; i < _capacity; ++i)
        {
            auto& v = _data[i];
            if (v.occupied)
            {
                using detail::unordered_map_node;
                v.~unordered_map_node<Key, Value>();
            }
        }
        _count = 0;
    }

    template <typename Key, typename Value, typename Hash>
    inline void unordered_map<Key, Value, Hash>::resize(const size_t newSize)
    {
        if (newSize > _capacity)
        {
            _resize(newSize);
        }
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::forward_iterator unordered_map<Key, Value, Hash>::find(const Key& k)
    {
        const size_t hash = _hasher(k);
        size_t idx = hash % _capacity;
        if (_data[idx].occupied && _data[idx].kv.first == k)
        {
            _data[idx].probe_count++;
            return forward_iterator(_data, idx, _capacity);
        }

        for (size_t i = 0; i < _capacity; ++i)
        {
            idx = (hash + i) % _capacity;
            size_t nextIdx = (hash + i + 1) % _capacity;
            if (_data[idx].occupied)
            {
                if (_data[nextIdx].occupied && _data[nextIdx].kv.first == k)
                {
                    _data[nextIdx].probe_count++;
                    if (_data[nextIdx].probe_count > _data[idx].probe_count)
                    {
                        swap(_data[nextIdx], _data[idx]);
                        return forward_iterator(_data, idx, _capacity);
                    }
                    else
                    {
                        return forward_iterator(_data, nextIdx, _capacity);
                    }
                }
            }
            else
            {
                break;
            }
        }

        return forward_iterator(_data, _capacity, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::const_forward_iterator unordered_map<Key, Value, Hash>::find(
        const Key& k) const
    {
        const size_t hash = _hasher(k);
        size_t idx = hash % _capacity;
        if (_data[idx].occupied && _data[idx].kv.first == k)
        {
            _data[idx].probe_count++;
            return const_forward_iterator(_data, idx, _capacity);
        }

        for (size_t i = 0; i < _capacity; ++i)
        {
            idx = (hash + i) % _capacity;
            size_t nextIdx = (hash + i + 1) % _capacity;
            if (_data[idx].occupied)
            {
                if (_data[nextIdx].occupied && _data[nextIdx].kv.first == k)
                {
                    _data[nextIdx].probe_count++;
                    if (_data[nextIdx].probe_count > _data[idx].probe_count)
                    {
                        swap(_data[nextIdx], _data[idx]);
                        return const_forward_iterator(_data, idx, _capacity);
                    }
                    else
                    {
                        return const_forward_iterator(_data, nextIdx, _capacity);
                    }
                }
            }
            else
            {
                break;
            }
        }

        return const_forward_iterator(_data, _capacity, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline Value& unordered_map<Key, Value, Hash>::operator[](const Key& k)
    {
        auto it = find(k);
        if (it == end())
        {
            it = insert({k, Value()});
        }
        return it->second;
    }

    template <typename Key, typename Value, typename Hash>
    inline Value& unordered_map<Key, Value, Hash>::operator[](Key&& k)
    {
        auto it = find(k);
        if (it == end())
        {
            it = insert({helios::move(k), Value()});
        }
        return it->second;
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::forward_iterator unordered_map<Key, Value, Hash>::insert(
        const pair<Key, Value>& kv)
    {
        auto idx = _insert(kv, _data, _capacity);
        return forward_iterator(_data, idx, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::forward_iterator unordered_map<Key, Value, Hash>::insert(
        pair<Key, Value>&& kv)
    {
        auto idx = _insert(std::forward<pair<Key, Value>>(kv), _data, _capacity);
        return forward_iterator(_data, idx, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline size_t unordered_map<Key, Value, Hash>::erase(const Key& k)
    {
        size_t hash = _hasher(k);
        size_t loc = _capacity;
        for (size_t i = 0; i < _capacity; ++i)
        {
            size_t searchIdx = (hash + i) % _capacity;
            if (_data[searchIdx].kv.first == k)
            {
                loc = searchIdx;
                break;
            }
        }

        if (loc != _capacity)
        {
            auto& toRemove = _data[loc];
            using detail::unordered_map_node;
            toRemove.~unordered_map_node<Key, Value>();
            ::new (_data + loc) detail::unordered_map_node<Key, Value>();
            _data[loc].occupied = false;
            _data[loc].probe_count = 0;

            size_t openSlot = loc;
            size_t searchLen = 1;

            for (i64 i = 1; i < _capacity; ++i)
            {
                const size_t searchIdx = (openSlot + i) % _capacity;
                if (!_data[searchIdx].occupied)
                {
                    searchLen = i;
                    break;
                }
            }

            for (i64 i = 1; i < searchLen; ++i)
            {
                const size_t searchIdx = (loc + i) % _capacity;
                const size_t hashValue = (_data[searchIdx].kv.first);
                const size_t curProbe = searchIdx >= hashValue ? (searchIdx - hashValue) : (_capacity - (hashValue - searchIdx));
                const size_t newProbe =
                    openSlot >= hashValue ? (openSlot - hashValue) : (_capacity - (hashValue - openSlot));

                if (newProbe < curProbe)
                {
                    _data[openSlot] = _data[searchIdx];
                    openSlot = searchIdx;
                    toRemove = _data[searchIdx];
                    toRemove.~unordered_map_node<Key, Value>();
                    ::new (_data + searchIdx) detail::unordered_map_node<Key, Value>();
                    toRemove.is_occupied = false;
                    toRemove.probe_count = 0;
                }
            }

            --_count;

            return 1;
        }

        return 0;
    }

    
    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::forward_iterator unordered_map<Key, Value, Hash>::begin()
    {
        return forward_iterator(_data, 0, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::const_forward_iterator unordered_map<Key, Value, Hash>::begin()
        const
    {
        return const_forward_iterator(_data, 0, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::forward_iterator unordered_map<Key, Value, Hash>::end()
    {
        return forward_iterator(_data, _capacity, _capacity);
    }

    template <typename Key, typename Value, typename Hash>
    inline typename unordered_map<Key, Value, Hash>::const_forward_iterator unordered_map<Key, Value, Hash>::end() const
    {
        return const_forward_iterator(_data, _capacity, _capacity);
    }
    
    template <typename Key, typename Value, typename Hash>
    inline void unordered_map<Key, Value, Hash>::_resize(const size_t newSize)
    {
        const size_t sizeBytes = newSize * sizeof(detail::unordered_map_node<Key, Value>);
        auto buffer = reinterpret_cast<detail::unordered_map_node<Key, Value>*>(new char[sizeBytes]);
        
        if (buffer == nullptr)
        {
            _data = buffer;
            return;
        }

        memset(buffer, 0, sizeBytes);

        for (i64 i = 0; i < _capacity; ++i)
        {
            if (_data[i].occupied)
            {
                _insert(_data[i].kv, buffer, newSize);
            }
        }

        delete[] reinterpret_cast<char*>(_data);
        _data = buffer;
        _capacity = newSize;
    }
    
    template <typename Key, typename Value, typename Hash>
    inline i64 unordered_map<Key, Value, Hash>::_insert(const pair<Key, Value>& kv,
                                                         detail::unordered_map_node<Key, Value>* buffer, size_t sz)
    {
        const size_t hashed = _hasher(kv.first);
        for (i64 i = 0; i < sz; ++i)
        {
            size_t idx = (hashed + i) % sz;
            
            if (!buffer[idx].occupied)
            {
                detail::unordered_map_node<Key, Value>* loc =
                    new (buffer + idx) detail::unordered_map_node<Key, Value>();
                loc->kv = kv;
                loc->occupied = true;
                loc->probe_count = 0;
                return idx;
            }
            else if (buffer[idx].kv.first == kv.first)
            {
                detail::unordered_map_node<Key, Value>* loc = buffer + idx;
                loc->kv = kv;
                loc->occupied = true;
                loc->probe_count = 0;
                return idx;
            }
        }
        return _capacity;
    }
    
    template <typename Key, typename Value, typename Hash>
    inline i64 unordered_map<Key, Value, Hash>::_insert(pair<Key, Value>&& kv,
                                                         detail::unordered_map_node<Key, Value>* buffer, size_t sz)
    {
        const size_t hashed = _hasher(kv.first);
        for (i64 i = 0; i < sz; ++i)
        {
            size_t idx = (hashed + i) % sz;

            if (!buffer[idx].occupied)
            {
                detail::unordered_map_node<Key, Value>* loc =
                    new (buffer + idx) detail::unordered_map_node<Key, Value>();
                loc->kv = helios::move(kv);
                loc->occupied = true;
                loc->probe_count = 0;
                ++_count;
                return idx;
            }
            else if (buffer[idx].kv.first == kv.first)
            {
                detail::unordered_map_node<Key, Value>* loc = buffer + idx;
                loc->kv = kv;
                loc->occupied = true;
                loc->probe_count = 0;
                ++_count;
                return idx;
            }
        }
        return _capacity;
    }
} // namespace helios