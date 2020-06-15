#pragma once

#include <helios/containers/initializer_list.hpp>
#include <helios/containers/memory.hpp>
#include <helios/macros.hpp>

namespace helios
{
    template <typename T, typename Allocator>
    class linked_list;

    template <typename T, typename Allocator>
    class linked_list_iterator
    {
        friend class linked_list<T, Allocator>;

        linked_list_iterator(T* ptr);

    public:
        linked_list_iterator(const linked_list_iterator&) = default;
        linked_list_iterator(linked_list_iterator&&) noexcept = default;
        ~linked_list_iterator() = default;
        linked_list_iterator& operator=(const linked_list_iterator&) = default;
        linked_list_iterator& operator=(linked_list_iterator&&) noexcept =
            default;

        bool operator==(const linked_list_iterator& it) const noexcept;
        bool operator!=(const linked_list_iterator& it) const noexcept;

        linked_list_iterator& operator++();
        linked_list_iterator operator++(i32);
        linked_list_iterator& operator--();
        linked_list_iterator operator--(i32);

        T& operator*();
        const T& operator*() const;

    private:
        linked_list_iterator* _prev; // non-owning pointer
        linked_list_iterator* _next; // non-owning pointer
        T* _value;                   // non-owning pointer
    };

    template <typename T, typename Allocator = allocator<T>>
    class linked_list
    {
    public:
        using iterator = linked_list_iterator<T, Allocator>;
        using const_iterator = const linked_list_iterator<T, Allocator>;
        using reference = T&;
        using const_reference = const T&;

        linked_list();
        linked_list(const linked_list& other);
        linked_list(linked_list&& other) noexcept;
        linked_list(std::initializer_list<T> ilist);
        ~linked_list();
        linked_list& operator=(const linked_list& other);
        linked_list& operator=(linked_list&& other) noexcept;
        linked_list& operator=(std::initializer_list<T> ilist);

        bool operator==(const linked_list& other) const noexcept;
        bool operator!=(const linked_list& other) const noexcept;

        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;

        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;

        bool empty() const noexcept;
        size_t size() const noexcept;

        template <typename... Arguments>
        void emplace_front(Arguments&&... args);
        void push_front(const T& value);
        void push_front(T&& value);

        template <typename... Arguments>
        void emplace_back(Arguments&&... args);
        void push_back(const T& value);
        void push_back(T&& value);

        template <typename... Arguments>
        iterator emplace(const_iterator position, Arguments&&... args);
        iterator insert(const_iterator position, const T& value);
        iterator insert(const_iterator position, T&& value);

        void pop_front();
        void pop_back();
        void erase(const_iterator iterator);
        void erase(const_iterator first, const_iterator last);
        void clear();

    private:
        size_t _count;
        Allocator _alloc;
        Allocator _node_alloc;

        linked_list_iterator<T, Allocator> _head;
        linked_list_iterator<T, Allocator> _tail;

        void _attach(linked_list_iterator<T, Allocator>& lhs,
                     linked_list_iterator<T, Allocator>& rhs,
                     linked_list_iterator<T, Allocator>& it);
        void _remove(linked_list_iterator<T, Allocator> it);
    };

    // implementation

    template <typename T, typename Allocator>
    linked_list_iterator<T, Allocator>::linked_list_iterator(T* ptr)
        : _prev(nullptr), _next(nullptr), _value(ptr)
    {
    }

    template <typename T, typename Allocator>
    bool linked_list_iterator<T, Allocator>::operator==(
        const linked_list_iterator<T, Allocator>& it) const noexcept
    {
        return _value == it._value && _next == it._next;
    }

    template <typename T, typename Allocator>
    bool linked_list_iterator<T, Allocator>::operator!=(
        const linked_list_iterator<T, Allocator>& it) const noexcept
    {
        return _value != it._value || _next != it._next;
    }

    template <typename T, typename Allocator>
    linked_list_iterator<T, Allocator>& linked_list_iterator<
        T, Allocator>::operator++()
    {
        this->_value = _next->_value;
        this->_next = _next->_next;
        this->_prev = this;

        return *this;
    }

    template <typename T, typename Allocator>
    linked_list_iterator<T, Allocator> linked_list_iterator<
        T, Allocator>::operator++(i32)
    {
        auto copy = *this;

        this->_value = _next->_value;
        this->_next = _next->_next;
        this->_prev = this;

        return copy;
    }

    template <typename T, typename Allocator>
    linked_list_iterator<T, Allocator>& linked_list_iterator<
        T, Allocator>::operator--()
    {
        this->_value = _prev->_value;
        this->_prev = _prev->_prev;
        this->_next = this;

        return *this;
    }

    template <typename T, typename Allocator>
    linked_list_iterator<T, Allocator> linked_list_iterator<
        T, Allocator>::operator--(i32)
    {
        auto copy = *this;

        this->_value = _prev->_value;
        this->_prev = _prev->_prev;
        this->_next = this;

        return copy;
    }

    template <typename T, typename Allocator>
    T& linked_list_iterator<T, Allocator>::operator*()
    {
        return *_value;
    }

    template <typename T, typename Allocator>
    const T& linked_list_iterator<T, Allocator>::operator*() const
    {
        return *_value;
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>::linked_list()
        : _count(0), _head(nullptr), _tail(nullptr)
    {
        _head._next = &_tail;
        _tail._prev = &_head;
        _head._prev = _tail._next = nullptr;
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>::linked_list(
        const linked_list<T, Allocator>& other)
        : _count(other._size), _head(nullptr), _tail(nullptr)
    {
        for (const T& value : other)
        {
            push_back(value);
        }
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>::linked_list(
        linked_list<T, Allocator>&& other) noexcept
        : _count(other._count), _head(nullptr), _tail(nullptr)
    {
        _head._prev = _tail._next = nullptr;
        _head._next = &other._head._next;
        _tail._prev = &other._tail._prev;
        other._head->_next = &other._tail;
        other._tail->_prev = &other._head;
        other._count = 0;
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>::linked_list(std::initializer_list<T> ilist)
        : _count(0), _head(nullptr), _tail(nullptr)
    {
        _head._next = &_tail;
        _tail._prev = &_head;
        _head._prev = _tail._next = nullptr;
        for (const T& value : ilist)
        {
            push_back(value);
        }
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>::~linked_list()
    {
        clear();
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>& linked_list<T, Allocator>::operator=(
        const linked_list<T, Allocator>& other)
    {
        clear();
        for (const T& value : other)
        {
            push_back(value);
        }
        return *this;
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>& linked_list<T, Allocator>::operator=(
        linked_list<T, Allocator>&& other) noexcept
    {
        clear();
        _head._prev = _tail._next = nullptr;
        _head._next = &other._head._next;
        _tail._prev = &other._tail._prev;
        other._head->_next = &other._tail;
        other._tail->_prev = &other._head;
        _count = other._count;
        other._count = 0;
        return *this;
    }

    template <typename T, typename Allocator>
    linked_list<T, Allocator>& linked_list<T, Allocator>::operator=(
        std::initializer_list<T> ilist)
    {
        clear();
        for (const T& value : ilist)
        {
            push_back(value);
        }
        return *this;
    }

    template <typename T, typename Allocator>
    bool linked_list<T, Allocator>::operator==(
        const linked_list<T, Allocator>& other) const noexcept
    {
        if (&other == this)
        {
            return true;
        }

        if (_count == other._count)
        {
            size_t i = 0;
            iterator me = begin();
            iterator ot = other.begin();

            while (i < _count)
            {
                if (*me != *ot)
                {
                    return false;
                }
                ++me;
                ++ot;
                ++i;
            }
        }
        return false;
    }

    template <typename T, typename Allocator>
    bool linked_list<T, Allocator>::operator!=(
        const linked_list<T, Allocator>& other) const noexcept
    {
        if (&other == this)
        {
            return false;
        }

        if (_count == other._count)
        {
            size_t i = 0;
            iterator me = begin();
            iterator ot = other.begin();

            while (i < _count)
            {
                if (*me != *ot)
                {
                    return true;
                }
                ++me;
                ++ot;
                ++i;
            }
        }
        return false;
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::reference linked_list<
        T, Allocator>::front()
    {
        return *(_head._next->_value);
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::const_reference linked_list<
        T, Allocator>::front() const
    {
        return *(_head._next->_value);
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::reference linked_list<T,
                                                              Allocator>::back()
    {
        return *(_tail._prev->_value);
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::const_reference linked_list<
        T, Allocator>::back() const
    {
        return *(_tail._prev->_value);
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::iterator linked_list<T,
                                                             Allocator>::begin()
    {
        return *(_head._next);
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::const_iterator linked_list<
        T, Allocator>::begin() const
    {
        return *(_head._next);
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::iterator linked_list<T,
                                                             Allocator>::end()
    {
        return _tail;
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::const_iterator linked_list<
        T, Allocator>::end() const
    {
        return _tail;
    }

    template <typename T, typename Allocator>
    bool linked_list<T, Allocator>::empty() const noexcept
    {
        return _count == 0;
    }

    template <typename T, typename Allocator>
    size_t linked_list<T, Allocator>::size() const noexcept
    {
        return _count;
    }

    template <typename T, typename Allocator>
    template <typename... Arguments>
    void linked_list<T, Allocator>::emplace_front(Arguments&&... args)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(
                ::new (ptr) T(helios::forward<Arguments>(args)...));
        _attach(_head, *(_head._next), *it);
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::push_front(const T& value)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(::new (ptr) T(value));
        _attach(_head, *(_head._next), *it);
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::push_front(T&& value)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(::new (ptr) T(value));
        _attach(_head, *(_head._next), *it);
    }

    template <typename T, typename Allocator>
    template <typename... Arguments>
    void linked_list<T, Allocator>::emplace_back(Arguments&&... args)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(
                ::new (ptr) T(helios::forward<Arguments>(args)...));
        _attach(*(_tail._prev), _tail, *it);
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::push_back(const T& value)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(::new (ptr) T(value));
        _attach(*(_tail._prev), _tail, *it);
        ++_count;
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::push_back(T&& value)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(::new (ptr) T(value));
        _attach(*(_tail._prev), _tail, *it);
        ++_count;
    }

    template <typename T, typename Allocator>
    template <typename... Arguments>
    typename linked_list<T, Allocator>::iterator linked_list<T, Allocator>::
        emplace(typename linked_list<T, Allocator>::const_iterator position,
                Arguments&&... args)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(
                ::new (ptr) T(helios::forward<Arguments>(args)...));
        _attach(*(position->_prev), position, *it);
        ++_count;
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::iterator linked_list<T, Allocator>::
        insert(typename linked_list<T, Allocator>::const_iterator position,
               const T& value)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(::new (ptr) T(value));
        _attach(*(position->_prev), position, *it);
        ++_count;
    }

    template <typename T, typename Allocator>
    typename linked_list<T, Allocator>::iterator linked_list<T, Allocator>::
        insert(typename linked_list<T, Allocator>::const_iterator position,
               T&& value)
    {
        T* ptr = _alloc.allocate(1);
        linked_list_iterator<T, Allocator>* it =
            new linked_list_iterator<T, Allocator>(::new (ptr) T(value));
        _attach(*(position->_prev), position, *it);
        ++_count;
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::pop_front()
    {
        auto& it = begin();
        it._value->~T();
        it.release(it._value);
        _remove(it);
        delete &it;
        --_count;
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::pop_back()
    {
        auto& it = --end();
        it._value->~T();
        _alloc.release(it._value);
        _remove(it);
        delete &it;
        --_count;
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::erase(
        typename linked_list<T, Allocator>::const_iterator iterator)
    {
        auto it_ptr = iterator._next->_prev;
        it_ptr->_value->~T();
        _alloc.release(it_ptr->_value);
        _remove(*it_ptr);
        delete it_ptr;
        --_count;
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::erase(
        typename linked_list<T, Allocator>::const_iterator first,
        typename linked_list<T, Allocator>::const_iterator last)
    {
        auto it = first;
        while (it != last)
        {
            auto tmp = it++;
            erase(tmp);
        }
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::clear()
    {
        erase(begin(), end());
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::_attach(
        linked_list_iterator<T, Allocator>& lhs,
        linked_list_iterator<T, Allocator>& rhs,
        linked_list_iterator<T, Allocator>& it)
    {
        lhs._next = &it;
        rhs._prev = &it;
        it._next = &rhs;
        it._prev = &lhs;
    }

    template <typename T, typename Allocator>
    void linked_list<T, Allocator>::_remove(
        linked_list_iterator<T, Allocator> it)
    {
        it._prev->_next = it._next;
        it._next->_prev = it._prev;
    }
} // namespace helios
