#pragma once

#include <helios/containers/initializer_list.hpp>
#include <helios/containers/memory.hpp>
#include <helios/containers/utility.hpp>

#if defined(_DEBUG)
#include <cassert>
#endif

#include <cstdlib>
#include <type_traits>

namespace helios
{
    template <typename Type, typename Allocator = allocator<Type>>
    class vector
    {
    public:
        using iterator = Type*;

        vector();
        vector(const vector& vec);
        vector(vector&& vec) noexcept;
        explicit vector(const size_t sz, const Type& value = Type());
        vector(std::initializer_list<Type> ilist);

        template <typename InputIterator>
        vector(InputIterator begin, InputIterator end);

        ~vector();

        vector& operator=(const vector& vec);
        vector& operator=(vector&& vec) noexcept;

        Type& at(const size_t elem);
        [[nodiscard]] const Type& at(const size_t elem) const;
        Type& operator[](const size_t elem);
        const Type& operator[](const size_t elem) const;
        Type& front();
        [[nodiscard]] const Type& front() const;
        Type& back();
        [[nodiscard]] const Type& back() const;
        Type* data();
        [[nodiscard]] auto data() const -> const Type*;

        [[nodiscard]] iterator begin() const noexcept;
        [[nodiscard]] iterator end() const noexcept;

        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] size_t max_size() const noexcept;
        void reserve(const size_t capacity);
        [[nodiscard]] size_t capacity() const noexcept;
        void shrink_to_fit();

        void clear();
        void insert(const iterator& it, const Type& value);

        template <typename... Args>
        void emplace(const iterator& it, Args&&... args);

        void erase(const iterator& it);
        void erase(const iterator& start, const iterator& end);

        void push_back(const Type& value);
        void push_back(Type&& value);

        template <typename... Args>
        void emplace_back(Args&&... args);

        void pop_back();
        void resize(const size_t sz);
        void swap(vector& other) noexcept;

    private:
        Type* _data;
        size_t _count;
        size_t _capacity;
        Allocator _allocator;

        Type* _allocate(const size_t sz);
        void _destroy(Type* src, const size_t count);
    };

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>::vector()
    {
        _data = nullptr;
        _count = 0;
        _capacity = 0;
    }

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>::vector(const vector& vec)
    {
        if (vec.empty())
        {
            _data = nullptr;
            _count = 0;
            _capacity = 0;
        }
        else
        {
            _data = _allocate(vec._capacity);
            _count = vec._count;
            _capacity = vec._capacity;

            for (size_t i = 0; i < vec._count; i++)
            {
                ::new (_data + i) Type(vec._data[i]);
            }
        }
    }

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>::vector(vector&& vec) noexcept
    {
        _data = vec._data;
        _count = vec._count;
        _capacity = vec._capacity;
        vec._data = nullptr;
        vec._count = 0;
        vec._capacity = 0;
    }

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>::vector(const size_t sz, const Type& value)
        : vector()
    {
        if (sz > 0)
        {
            _data = _allocate(sz);
            _capacity = sz;
            _count = sz;

            for (size_t i = 0; i < sz; i++)
            {
                _data[i] = value;
            }
        }
    }

    template <typename Type, typename Allocator>
    vector<Type, Allocator>::vector(std::initializer_list<Type> ilist)
        : vector()
    {
        for (const auto it : ilist)
        {
            push_back(it);
        }
    }

    template <typename Type, typename Allocator>
    template <typename InputIterator>
    vector<Type, Allocator>::vector(InputIterator begin, InputIterator end)
        : vector()
    {
        for (InputIterator it = begin; it != end; ++it)
        {
            push_back(*it);
        }
    }

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>::~vector()
    {
        if (_data)
        {
            _destroy(_data, _count);
            _allocator.release(_data);
            _data = nullptr;
        }
    }

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>& vector<Type, Allocator>::operator=(
        const vector& vec)
    {
        if (this != &vec)
        {
            if (_data)
            {
                _destroy(_data, _count);
                _allocator.release(_data);
            }

            if (vec.empty())
            {
                _data = nullptr;
                _count = _capacity = 0;
            }
            else
            {
                _data = _allocate(vec._capacity);
                _count = vec._count;
                _capacity = vec._capacity;

                for (size_t i = 0; i < _count; i++)
                {
                    ::new (_data + i) Type(vec._data[i]);
                }
            }
        }
        return *this;
    }

    template <typename Type, typename Allocator>
    inline vector<Type, Allocator>& vector<Type, Allocator>::operator=(
        vector&& vec) noexcept
    {
        if (this != &vec)
        {
            if (_data)
            {
                _destroy(_data, _count);
                _allocator.release(_data);
            }

            _data = vec._data;
            _count = vec._count;
            _capacity = vec._capacity;

            vec._data = nullptr;
            vec._count = 0;
            vec._capacity = 0;
        }

        return *this;
    }

    template <typename Type, typename Allocator>
    inline Type& vector<Type, Allocator>::at(const size_t elem)
    {
#if defined(_DEBUG)
        assert(elem < _count);
#endif
        return _data[elem];
    }

    template <typename Type, typename Allocator>
    inline const Type& vector<Type, Allocator>::at(const size_t elem) const
    {
#if defined(_DEBUG)
        assert(elem < _count);
#endif
        return _data[elem];
    }

    template <typename Type, typename Allocator>
    inline Type& vector<Type, Allocator>::operator[](const size_t elem)
    {
        return at(elem);
    }

    template <typename Type, typename Allocator>
    inline const Type& vector<Type, Allocator>::operator[](
        const size_t elem) const
    {
        return at(elem);
    }

    template <typename Type, typename Allocator>
    inline Type& vector<Type, Allocator>::front()
    {
        return at(0);
    }

    template <typename Type, typename Allocator>
    inline const Type& vector<Type, Allocator>::front() const
    {
        return at(0);
    }

    template <typename Type, typename Allocator>
    inline Type& vector<Type, Allocator>::back()
    {
        return at(_count - 1);
    }

    template <typename Type, typename Allocator>
    inline const Type& vector<Type, Allocator>::back() const
    {
        return at(_count - 1);
    }

    template <typename Type, typename Allocator>
    inline Type* vector<Type, Allocator>::data()
    {
        return _data;
    }

    template <typename Type, typename Allocator>
    inline const Type* vector<Type, Allocator>::data() const
    {
        return _data;
    }

    template <typename Type, typename Allocator>
    typename vector<Type, Allocator>::iterator vector<Type, Allocator>::begin()
        const noexcept
    {
        return _data;
    }

    template <typename Type, typename Allocator>
    typename vector<Type, Allocator>::iterator vector<Type, Allocator>::end()
        const noexcept
    {
        return _data + _count;
    }

    template <typename Type, typename Allocator>
    inline bool vector<Type, Allocator>::empty() const noexcept
    {
        return _count == 0;
    }

    template <typename Type, typename Allocator>
    inline size_t vector<Type, Allocator>::size() const noexcept
    {
        return _count;
    }

    template <typename Type, typename Allocator>
    inline size_t vector<Type, Allocator>::max_size() const noexcept
    {
        return ~static_cast<size_t>(0);
    }

    template <typename Type, typename Allocator>
    inline void vector<Type, Allocator>::reserve(const size_t capacity)
    {
        if (capacity > _capacity)
        {
            Type* data = _allocator.allocate(capacity);

            if constexpr (std::is_nothrow_move_constructible<Type>::value)
            {
                for (size_t i = 0; i < _count; i++)
                {
                    ::new (data + i) Type(helios::move(_data[i]));
                }
            }
            else
            {
                for (size_t i = 0; i < _count; i++)
                {
                    ::new (data + i) Type(_data[i]);
                }
            }
            _allocator.release(_data);
            _data = data;
            _capacity = capacity;
        }
    }

    template <typename Type, typename Allocator>
    inline size_t vector<Type, Allocator>::capacity() const noexcept
    {
        return _capacity;
    }

    template <typename Type, typename Allocator>
    inline void vector<Type, Allocator>::shrink_to_fit()
    {
        Type* data = _allocate(_count);
        for (size_t i = 0; i < _count; i++)
        {
            ::new (data + i) Type(helios::move(_data[i]));
        }

        _allocator.release(_data);
        _data = data;
        _capacity = _count;
    }

    template <typename Type, typename Allocator>
    inline void vector<Type, Allocator>::clear()
    {
        _destroy(_data, _count);
        _count = 0;
    }

    template <typename Type, typename Allocator>
    inline void vector<Type, Allocator>::insert(const iterator& it,
                                                const Type& value)
    {
        if (_count == _capacity)
        {
            reserve(_capacity + 1);
        }
        const size_t location = it - _data;
        for (size_t i = _count; i != location; --i)
        {
            ::new (_data + i) Type(helios::move(_data[i - 1]));
        }
        ::new (_data + location) Type(value);
        _count++;
    }

    template <typename Type, typename Allocator>
    template <typename... Args>
    void vector<Type, Allocator>::emplace(const iterator& it, Args&&... args)
    {
        if (_count == _capacity)
        {
            reserve(_capacity + 1);
        }
        const size_t location = it - _data;
        for (size_t i = _count; i != location; --i)
        {
            ::new (_data + i) Type(helios::move(_data[i - 1]));
        }
        ::new (_data + location) Type(helios::forward<Args>(args)...);
        ++_count;
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::erase(const iterator& it)
    {
        erase(it, it + 1);
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::erase(const iterator& start,
                                        const iterator& end)
    {
#if defined(_DEBUG)
        assert(start != end);
#endif
        // Element Count
        const size_t count = static_cast<size_t>(end - start);

        const size_t e = static_cast<size_t>(end - _data);
        for (size_t i = static_cast<size_t>(start - _data); i < e; ++i)
        {
            _data[i].~Type();
            if (i + count < _count)
            {
                ::new (_data + i) Type(helios::move(_data[i + count]));
            }
            --_count;
        }
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::push_back(const Type& value)
    {
        if (_count == _capacity)
        {
            reserve(_capacity + 1);
        }
        ::new (_data + _count++) Type(value);
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::push_back(Type&& value)
    {
        if (_count == _capacity)
        {
            reserve(_capacity + 1);
        }
        ::new (_data + _count++) Type(helios::move(value));
    }

    template <typename Type, typename Allocator>
    template <typename... Args>
    void vector<Type, Allocator>::emplace_back(Args&&... args)
    {
        if (_count == _capacity)
        {
            reserve(_capacity + 1);
        }
        ::new (_data + _count++) Type(helios::forward<Args>(args)...);
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::pop_back()
    {
        erase(vector_forward_iterator(_data, _count - 1, _count));
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::resize(const size_t sz)
    {
        if (sz != _count)
        {
            Type* data = _allocate(sz);
            const size_t toMove = sz > _count ? _count : sz;
            for (size_t i = 0; i < toMove; i++)
            {
                data[i] = helios::move(_data[i]);
            }
            for (size_t i = toMove; i < _count; i++)
            {
                _data[i].~Type();
            }
            const size_t toDefaultConstruct = sz > _count ? sz - _count : 0;
            for (size_t i = toMove; i < toMove + toDefaultConstruct; i++)
            {
                ::new (data + i) Type();
            }

            _allocator.release(_data);
            _data = data;
            _count = sz;
            _capacity = sz;
        }
    }

    template <typename Type, typename Allocator>
    void vector<Type, Allocator>::swap(vector& other) noexcept
    {
        auto data = _data;
        _data = other._data;
        other._data = data;

        auto count = _count;
        _count = other._count;
        other._count = count;

        auto cap = _capacity;
        _capacity = other._capacity;
        other._capacity = cap;
    }

    template <typename Type, typename Allocator>
    inline Type* vector<Type, Allocator>::_allocate(const size_t sz)
    {
        return _allocator.allocate(sz);
    }

    template <typename Type, typename Allocator>
    inline void vector<Type, Allocator>::_destroy(Type* src, const size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            src[i].~Type();
        }
    }
} // namespace helios
