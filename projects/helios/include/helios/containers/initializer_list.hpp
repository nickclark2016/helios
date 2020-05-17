#pragma once

#if defined(COMPILER_CANNOT_GENERATE_INITIALIZER_LIST)
namespace std
{
    template <typename Type>
    class initializer_list
    {
    public:
        typedef Type value_type;
        typedef const Type& reference;
        typedef const Type& const_reference;
        typedef size_t size_type;
        typedef const Type* iterator;
        typedef const Type* const_iterator;

        constexpr initializer_list() noexcept : _array(0), _size(0)
        {
        }

        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return _size;
        }

        constexpr const_iterator begin() const noexcept
        {
            return _array;
        }

        constexpr const_iterator end() const noexcept
        {
            return begin() + size();
        }

    private:
        constexpr initializer_list(const_iterator arr, size_type sz)
            : _array(arr), _size(sz)
        {
        }

        iterator _array;
        size_type _size;
    };

    template <class Type>
    constexpr const Type* begin(initializer_list<Type> list) noexcept
    {
        return list.begin();
    }

    template <class Type>
    constexpr const Type* end(initializer_list<Type> list) noexcept
    {
        return list.end();
    }
} // namespace std
#else
#include <initializer_list>
#endif