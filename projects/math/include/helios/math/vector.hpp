#pragma once

#include <helios/containers/utility.hpp>
#include <helios/macros.hpp>
#include <helios/math/utils.hpp>

#include <cmath>
#include <xmmintrin.h>

namespace helios
{
    struct alignas(16) Vector2f
    {
        union
        {
            struct
            {
                f32 x;
                f32 y;
            };
            struct
            {
                f32 u;
                f32 v;
            };
            struct
            {
                f32 r;
                f32 g;
            };
            f32 data[4]; // for intrinsics, we need 4 floats
        };

        constexpr Vector2f() noexcept;
        constexpr explicit Vector2f(const f32 scalar) noexcept;
        constexpr Vector2f(const f32 x, const f32 y) noexcept;
        constexpr Vector2f(const Vector2f& other) noexcept;
        constexpr Vector2f(Vector2f&& other) noexcept;
        ~Vector2f() = default;
        constexpr Vector2f& operator=(const f32 rhs) noexcept;
        constexpr Vector2f& operator=(const Vector2f& rhs) noexcept;
        constexpr Vector2f& operator=(Vector2f&& rhs) noexcept;
        constexpr bool operator==(const f32 rhs) noexcept;
        constexpr bool operator==(const Vector2f& rhs) noexcept;
        constexpr bool operator!=(const f32 rhs) noexcept;
        constexpr bool operator!=(const Vector2f& rhs) noexcept;
        Vector2f& operator+=(const f32 rhs) noexcept;
        Vector2f& operator+=(const Vector2f& rhs) noexcept;
        Vector2f& operator-=(const f32 rhs) noexcept;
        Vector2f& operator-=(const Vector2f& rhs) noexcept;
        Vector2f& operator*=(const f32 rhs) noexcept;
        Vector2f& operator*=(const Vector2f& rhs) noexcept;
        Vector2f& operator/=(const f32 rhs) noexcept;
        Vector2f& operator/=(const Vector2f& rhs) noexcept;
        HELIOS_NO_DISCARD constexpr Vector2f abs() const noexcept;
        HELIOS_NO_DISCARD f32 angle(const Vector2f& other) const noexcept;
        HELIOS_NO_DISCARD constexpr f32 dot(
            const Vector2f& other) const noexcept;
        HELIOS_NO_DISCARD f32 euclidianNorm() const noexcept;
        HELIOS_NO_DISCARD f32 length() const noexcept;
        HELIOS_NO_DISCARD f32 magnitude() const noexcept;
        HELIOS_NO_DISCARD constexpr f32 norm1() const noexcept;
        HELIOS_NO_DISCARD f32 norm2() const noexcept;
        HELIOS_NO_DISCARD Vector2f reflect(const Vector2f& line) const noexcept;
    };

    HELIOS_NO_DISCARD Vector2f operator+(const f32 lhs, const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator+(const Vector2f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector2f operator+(const Vector2f lhs,
                                         const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator-(const f32 lhs, const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator-(const Vector2f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector2f operator-(const Vector2f lhs,
                                         const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator*(const f32 lhs, const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator*(const Vector2f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector2f operator*(const Vector2f lhs,
                                         const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator/(const f32 lhs, const Vector2f& rhs);
    HELIOS_NO_DISCARD Vector2f operator/(const Vector2f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector2f operator/(const Vector2f lhs,
                                         const Vector2f& rhs);
    HELIOS_NO_DISCARD constexpr Vector2f abs(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 angle(const Vector2f& lhs,
                                const Vector2f& rhs) noexcept;
    HELIOS_NO_DISCARD constexpr f32 dot(const Vector2f& lhs,
                                        const Vector2f& rhs) noexcept;
    HELIOS_NO_DISCARD f32 euclidianNorm(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 length(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 magnitude(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD constexpr f32 norm1(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm2(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD Vector2f reflect(const Vector2f vec,
                                       const Vector2f& line) noexcept;

    constexpr Vector2f::Vector2f() noexcept : Vector2f(0.0f){};

    constexpr Vector2f::Vector2f(const f32 scalar) noexcept
        : Vector2f(scalar, scalar)
    {
    }

    constexpr Vector2f::Vector2f(const f32 x, const f32 y) noexcept : data()
    {
        data[0] = x;
        data[1] = y;
    }

    constexpr Vector2f::Vector2f(const Vector2f& other) noexcept
        : Vector2f(other.x, other.y)
    {
    }

    constexpr Vector2f::Vector2f(Vector2f&& other) noexcept
        : Vector2f(helios::move(other.x), helios::move(other.y))
    {
    }

    constexpr Vector2f& Vector2f::operator=(const f32 rhs) noexcept
    {
        data[0] = data[1] = rhs;
        return *this;
    }

    constexpr Vector2f& Vector2f::operator=(const Vector2f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        return *this;
    }

    constexpr Vector2f& Vector2f::operator=(Vector2f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        return *this;
    }

    constexpr bool Vector2f::operator==(const f32 rhs) noexcept
    {
        return data[0] == rhs && data[1] == rhs;
    }

    constexpr bool Vector2f::operator==(const Vector2f& rhs) noexcept
    {
        return data[0] == rhs.data[0] && data[1] == rhs.data[1];
    }

    constexpr bool Vector2f::operator!=(const f32 rhs) noexcept
    {
        return data[0] != rhs || data[1] != rhs;
    }

    constexpr bool Vector2f::operator!=(const Vector2f& rhs) noexcept
    {
        return data[0] != rhs.data[0] || data[1] != rhs.data[1];
    }

    constexpr Vector2f Vector2f::abs() const noexcept
    {
        return Vector2f(helios::abs(data[0]), helios::abs(data[1]));
    }

    constexpr f32 Vector2f::dot(const Vector2f& other) const noexcept
    {
        return data[0] * other.data[0] + data[1] * other.data[1];
    }

    constexpr f32 Vector2f::norm1() const noexcept
    {
        return dot(*this);
    }

    constexpr Vector2f abs(const Vector2f& vec) noexcept
    {
        return vec.abs();
    }

    constexpr f32 dot(const Vector2f& lhs, const Vector2f& rhs) noexcept
    {
        return lhs.dot(rhs);
    }

    constexpr f32 norm1(const Vector2f& vec) noexcept
    {
        return vec.norm1();
    }

    struct Vector3f
    {
    };

    struct Vector4f
    {
    };
} // namespace helios
