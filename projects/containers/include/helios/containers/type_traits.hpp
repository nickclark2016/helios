#pragma once

namespace helios
{
    template <typename T, T v>
    struct integral_constant
    {
        using value_type = T;
        using type = integral_constant<T, v>;
        static constexpr T value = v;

        inline constexpr operator value_type() const noexcept
        {
            return value;
        }

        inline constexpr value_type operator()() const noexcept
        {
            return value;
        }
    };

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template <typename T>
    struct remove_reference
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T&>
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T&&>
    {
        using type = T;
    };

    template <typename T>
    struct remove_const
    {
        using type = T;
    };

    template <typename T>
    struct remove_const<const T>
    {
        using type = T;
    };

    template <typename T>
    using remove_const_t = typename remove_const<T>::type;

    template <typename T>
    struct remove_volatile
    {
        using type = T;
    };

    template <typename T>
    struct remove_volatile<volatile T>
    {
        using type = T;
    };

    template <typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    template <typename T>
    struct remove_cv
    {
        using type = T;
    };

    template <typename T>
    struct remove_cv<const T>
    {
        using type = T;
    };

    template <typename T>
    struct remove_cv<volatile T>
    {
        using type = T;
    };

    template <typename T>
    struct remove_cv<const volatile T>
    {
        using type = T;
    };

    template <typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template <typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template <typename T>
    struct is_lvalue_reference : false_type
    {
    };

    template <typename T>
    struct is_lvalue_reference<T&> : true_type
    {
    };

    template <typename T>
    constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

    template <typename T>
    struct is_class : integral_constant<bool, __is_class(T)>
    {
    };

    template <typename T>
    constexpr bool is_class_v = is_class<T>::value;

    namespace detail
    {
        template <typename T>
        struct is_pointer_helper : false_type
        {
        };

        template <typename T>
        struct is_pointer_helper<T*> : true_type
        {
        };
    }

    template <typename T>
    struct is_pointer : detail::is_pointer_helper<remove_cv_t<T>>
    {
    };

    template <typename T>
    constexpr bool is_pointer_v = is_pointer<T>::value;

    template <typename Base, typename Derived>
    struct is_base_of
        : integral_constant<bool, is_class_v<Base> &&
                                      is_class_v<Derived>&& __is_base_of(
                                          Base, Derived)>
    {
    };

    template <typename Base, typename Derived>
    constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

    template <typename T1, typename T2>
    struct is_same : false_type
    {
    };

    template <typename T>
    struct is_same<T, T> : true_type
    {
    };

    template <typename T1, typename T2>
    constexpr bool is_same_v = is_same<T1, T2>::value;

    template <bool B, typename T, typename F>
    struct conditional
    {
        using type = T;
    };

    template <typename T, typename F>
    struct conditional<false, T, F>
    {
        using type = F;
    };

    template <bool B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;
} // namespace helios