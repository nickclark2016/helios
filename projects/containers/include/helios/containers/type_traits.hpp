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
    static constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

    template <typename T>
    struct is_class : integral_constant<bool, __is_class(T)>
    {
    };

    template <typename T>
    static constexpr bool is_class_v = is_class<T>::value;

    template <typename Base, typename Derived>
    struct is_base_of : integral_constant<bool, is_class_v<Base> && is_class_v<Derived>&& __is_base_of(Base, Derived)>
    {
    };

    template <typename Base, typename Derived>
    static constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

    template <typename T1, typename T2>
    struct is_same : false_type
    {
    };

    template <typename T>
    struct is_same<T, T> : true_type
    {
    };

    template <typename T1, typename T2>
    static constexpr bool is_same_v = is_same<T1, T2>::value;

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