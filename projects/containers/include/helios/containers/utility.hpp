#pragma once

#include <helios/containers/type_traits.hpp>

#include <cstddef>
#include <type_traits>

namespace helios
{
    template <typename Type>
    inline Type&& forward(remove_reference_t<Type>& t) noexcept
    {
        return static_cast<Type&&>(t);
    }

    template <typename Type>
    inline Type&& forward(remove_reference_t<Type>&& t) noexcept
    {
        static_assert(!is_lvalue_reference_v<Type>,
                      "Cannot forward an rvalue as an lvalue.");
        return static_cast<Type&&>(t);
    }

    template <typename Type>
    inline constexpr remove_reference_t<Type>&& move(Type&& t) noexcept
    {
        return static_cast<remove_reference_t<Type>&&>(t);
    }

    struct in_place_t
    {
        explicit in_place_t() = default;
    };

    constexpr in_place_t in_place{};

    template <class T>
    struct in_place_type_t
    {

        explicit in_place_type_t() = default;
    };

    template <class T>
    constexpr in_place_type_t<T> in_place_type{};

    template <size_t I>
    struct in_place_index_t
    {

        explicit in_place_index_t() = default;
    };

    template <size_t I>
    constexpr in_place_index_t<I> in_place_index{};

    template <typename Output, typename Input>
    Output cast(Input ptr)
    {
        static_assert(std::is_pointer_v<Input> && std::is_pointer_v<Output>,
                      "Input and Output must both be pointers.");
#if defined(_DEBUG)
        return dynamic_cast<Output>(ptr);
#else
        return reinterpret_cast<Output>(ptr);
#endif
    }

    template <typename Tp>
    inline constexpr Tp max(Tp left, Tp right)
    {
        return left > right ? left : right;
    }

    template <typename Tp>
    inline constexpr Tp min(Tp left, Tp right)
    {
        return left < right ? left : right;
    }

    namespace detail
    {
        constexpr unsigned long long hash_prime = 2654435761ULL;
        constexpr unsigned long long longer_hash_prime = 11400714819323198549ULL;
    } // namespace detail

    template <typename Tp>
    struct hash
    {
    };

    template<>
    struct hash<bool>
    {
        size_t operator()(const bool& value) const noexcept;
    };

    template <>
    struct hash<unsigned char>
    {
        size_t operator()(const unsigned char& value) const noexcept;
    };

    template <>
    struct hash<char>
    {
        size_t operator()(const char& value) const noexcept;
    };

    template <>
    struct hash<unsigned short>
    {
        size_t operator()(const unsigned short& value) const noexcept;
    };

    template <>
    struct hash<short>
    {
        size_t operator()(const short& value) const noexcept;
    };

    template <>
    struct hash<unsigned int>
    {
        size_t operator()(const unsigned int& value) const noexcept;
    };

    template <>
    struct hash<int>
    {
        size_t operator()(const int& value) const noexcept;
    };

    template <>
    struct hash<unsigned long>
    {
        size_t operator()(const unsigned long& value) const noexcept;
    };

    template <>
    struct hash<long>
    {
        size_t operator()(const long& value) const noexcept;
    };

    template <>
    struct hash<unsigned long long>
    {
        size_t operator()(const unsigned long long& value) const noexcept;
    };

    template <>
    struct hash<long long>
    {
        size_t operator()(const long long& value) const noexcept;
    };

    template <>
    struct hash<float>
    {
        size_t operator()(const float& value) const noexcept;
    };

    template <>
    struct hash<double>
    {
        size_t operator()(const double& value) const noexcept;
    };

    template <>
    struct hash<long double>
    {
        size_t operator()(const long double& value) const noexcept;
    };

    template <>
    struct hash<std::nullptr_t>
    {
        size_t operator()(const std::nullptr_t& value) const noexcept;
    };

    template <typename T>
    struct hash<T*>
    {
        size_t operator()(const T* value) const noexcept;
    };
    
    inline size_t hash<bool>::operator()(const bool& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<unsigned char>::operator()(const unsigned char& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<char>::operator()(const char& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<unsigned short>::operator()(const unsigned short& value) const noexcept
    {
        return static_cast<size_t>(value);
    }

    inline size_t hash<short>::operator()(const short& value) const noexcept
    {
        return static_cast<size_t>(value);
    }

    inline size_t hash<unsigned int>::operator()(const unsigned int& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<int>::operator()(const int& value) const noexcept
    {
        return static_cast<size_t>(value);
    }

    inline size_t hash<unsigned long>::operator()(const unsigned long& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<long>::operator()(const long& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<unsigned long long>::operator()(const unsigned long long& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<long long>::operator()(const long long& value) const noexcept
    {
        return static_cast<size_t>(value);
    }
    
    inline size_t hash<std::nullptr_t>::operator()(const std::nullptr_t&) const noexcept
    {
        return 0;
    }

    inline size_t hash<float>::operator()(const float& value) const noexcept
    {
        union
        {
            float f;
            size_t b;
        } u;
        u.b = 0;
        u.f = value;
        return u.b * detail::hash_prime;
    }

    inline size_t hash<double>::operator()(const double& value) const noexcept
    {
        union
        {
            double f;
            size_t b;
        } u;
        u.b = 0;
        u.f = value;
        return u.b * detail::longer_hash_prime;
    }

    inline size_t hash<long double>::operator()(const long double& value) const noexcept
    {
        union
        {
            long double f;
            size_t b;
        } u;
        u.b = 0;
        u.f = value;
        return u.b * detail::longer_hash_prime;
    }
    
    template <typename T>
    inline size_t hash<T*>::operator()(const T* value) const noexcept
    {
        // most allocations are 8 byte aligned, so trim off the 3 LSBs
        return static_cast<size_t>(value) >> 3;
    }

    template <typename First, typename Second>
    struct pair
    {
        First first;
        Second second;
    };

    template <typename First, typename Second>
    pair(First, Second) -> pair<First, Second>;
} // namespace helios
