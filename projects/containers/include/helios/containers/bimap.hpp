#pragma once

#include <map>

namespace helios
{
    template <typename Key, typename Value>
    class bimap
    {
    public:
        bimap() = default;
        bimap(const bimap&) = default;
        bimap(bimap&&) noexcept = default;
        ~bimap() = default;
        bimap& operator=(const bimap&) = default;
        bimap& operator=(bimap&&) noexcept = default;

        auto at(const Key& key);
        const auto at(const Key& key) const;
        auto at(const Value& value);
        const auto at(const Value& value) const;

        auto operator[](const Key& key);
        const auto operator[](const Key& key) const;
        auto operator[](const Value& value);
        const auto operator[](const Value& value) const;

        void insert(const std::pair<Key, Value>& pair);
        void clear();

    private:
        std::map<Key, Value> _keyMap;
        std::map<Value, Key> _valueMap;
    };

    template <typename Key, typename Value>
    inline auto bimap<Key, Value>::at(const Key& key)
    {
        return _keyMap.at(key);
    }
    template <typename Key, typename Value>
    inline const auto bimap<Key, Value>::at(const Key& key) const
    {
        return _keyMap.at(key);
    }

    template <typename Key, typename Value>
    inline auto bimap<Key, Value>::at(const Value& value)
    {
        return _valueMap.at(value);
    }

    template <typename Key, typename Value>
    inline const auto bimap<Key, Value>::at(const Value& value) const
    {
        return _valueMap.at(value);
    }

    template <typename Key, typename Value>
    inline auto bimap<Key, Value>::operator[](const Key& key)
    {
        return at(key);
    }

    template <typename Key, typename Value>
    inline const auto bimap<Key, Value>::operator[](const Key& key) const
    {
        return at(key);
    }

    template <typename Key, typename Value>
    inline auto bimap<Key, Value>::operator[](const Value& value)
    {
        return at(value);
    }

    template <typename Key, typename Value>
    inline const auto bimap<Key, Value>::operator[](const Value& value) const
    {
        return at(value);
    }

    template <typename Key, typename Value>
    inline void bimap<Key, Value>::insert(const std::pair<Key, Value>& pair)
    {
        _keyMap.insert(pair);
        _valueMap.insert({pair.second, pair.first});
    }

    template <typename Key, typename Value>
    inline void bimap<Key, Value>::clear()
    {
        _keyMap.clear();
        _valueMap.clear();
    }
} // namespace helios