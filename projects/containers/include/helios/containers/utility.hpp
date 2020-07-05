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
        static_assert(!is_lvalue_reference_v<Type>, "Cannot forward an rvalue as an lvalue.");
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

    inline constexpr in_place_t in_place{};

    template <class T>
    struct in_place_type_t
    {

        explicit in_place_type_t() = default;
    };

    template <class T>
    inline constexpr in_place_type_t<T> in_place_type{};

    template <size_t I>
    struct in_place_index_t
    {

        explicit in_place_index_t() = default;
    };

    template <size_t I>
    inline constexpr in_place_index_t<I> in_place_index{};

    template <typename Output, typename Input>
    Output cast(Input ptr)
    {
        static_assert(std::is_pointer_v<Input> && std::is_pointer_v<Output>, "Input and Output must both be pointers.");
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
} // namespace helios
