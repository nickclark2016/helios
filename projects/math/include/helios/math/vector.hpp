#pragma once

#include <helios/containers/utility.hpp>
#include <helios/macros.hpp>
#include <helios/math/utils.hpp>

#include <cmath>

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
            struct
            {
                f32 s;
                f32 t;
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
        constexpr bool operator==(const f32 rhs) const noexcept;
        constexpr bool operator==(const Vector2f& rhs) const noexcept;
        constexpr bool operator!=(const f32 rhs) const noexcept;
        constexpr bool operator!=(const Vector2f& rhs) const noexcept;
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
        HELIOS_NO_DISCARD f32 dot(const Vector2f& other) const noexcept;
        HELIOS_NO_DISCARD f32 euclidianNorm() const noexcept;
        HELIOS_NO_DISCARD f32 length() const noexcept;
        HELIOS_NO_DISCARD f32 magnitude() const noexcept;
        HELIOS_NO_DISCARD f32 norm1() const noexcept;
        HELIOS_NO_DISCARD f32 norm2() const noexcept;
        HELIOS_NO_DISCARD Vector2f reflect(const Vector2f& line) const noexcept;
    };

    constexpr bool operator==(const f32 lhs, const Vector2f& rhs);
    constexpr bool operator!=(const f32 lhs, const Vector2f& rhs);
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
    HELIOS_NO_DISCARD f32 dot(const Vector2f& lhs,
                              const Vector2f& rhs) noexcept;
    HELIOS_NO_DISCARD f32 euclidianNorm(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 length(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 magnitude(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm1(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm2(const Vector2f& vec) noexcept;
    HELIOS_NO_DISCARD Vector2f reflect(const Vector2f vec,
                                       const Vector2f& line) noexcept;

    struct alignas(16) Vector3f
    {
        union
        {
            struct
            {
                f32 x;
                f32 y;
                f32 z;
            };
            struct
            {
                f32 r;
                f32 g;
                f32 b;
            };
            struct
            {
                f32 u;
                f32 v;
                f32 w;
            };
            struct
            {
                f32 s;
                f32 t;
                f32 p;
            };
            f32 data[4];
        };

        constexpr Vector3f() noexcept;
        constexpr explicit Vector3f(const f32 scalar) noexcept;
        constexpr Vector3f(const f32 x, const f32 y, const f32 z) noexcept;
        constexpr Vector3f(const Vector2f& xy, const f32 z = 0.0f);
        constexpr Vector3f(const Vector3f& other) noexcept;
        constexpr Vector3f(Vector2f&& xy, const f32 z = 0.0f);
        constexpr Vector3f(Vector3f&& other) noexcept;
        ~Vector3f() = default;
        constexpr Vector3f& operator=(const f32 rhs) noexcept;
        constexpr Vector3f& operator=(const Vector2f& rhs) noexcept;
        constexpr Vector3f& operator=(const Vector3f& rhs) noexcept;
        constexpr Vector3f& operator=(Vector2f&& rhs) noexcept;
        constexpr Vector3f& operator=(Vector3f&& rhs) noexcept;
        constexpr bool operator==(const f32 rhs) const noexcept;
        constexpr bool operator==(const Vector3f& rhs) const noexcept;
        constexpr bool operator!=(const f32 rhs) const noexcept;
        constexpr bool operator!=(const Vector3f& rhs) const noexcept;
        Vector3f& operator+=(const f32 rhs) noexcept;
        Vector3f& operator+=(const Vector3f& rhs) noexcept;
        Vector3f& operator-=(const f32 rhs) noexcept;
        Vector3f& operator-=(const Vector3f& rhs) noexcept;
        Vector3f& operator*=(const f32 rhs) noexcept;
        Vector3f& operator*=(const Vector3f& rhs) noexcept;
        Vector3f& operator/=(const f32 rhs) noexcept;
        Vector3f& operator/=(const Vector3f& rhs) noexcept;
        HELIOS_NO_DISCARD constexpr Vector3f abs() const noexcept;
        HELIOS_NO_DISCARD f32 angle(const Vector3f& other) const noexcept;
        HELIOS_NO_DISCARD Vector3f cross(const Vector3f& other) const noexcept;
        HELIOS_NO_DISCARD f32 dot(const Vector3f& other) const noexcept;
        HELIOS_NO_DISCARD f32 euclidianNorm() const noexcept;
        HELIOS_NO_DISCARD f32 length() const noexcept;
        HELIOS_NO_DISCARD f32 magnitude() const noexcept;
        HELIOS_NO_DISCARD f32 norm1() const noexcept;
        HELIOS_NO_DISCARD f32 norm2() const noexcept;
        HELIOS_NO_DISCARD Vector3f reflect(const Vector3f& line) const noexcept;
    };

    constexpr bool operator==(const f32 lhs, const Vector3f& rhs);
    constexpr bool operator!=(const f32 lhs, const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator+(const f32 lhs, const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator+(const Vector3f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector3f operator+(const Vector3f lhs,
                                         const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator-(const f32 lhs, const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator-(const Vector3f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector3f operator-(const Vector3f lhs,
                                         const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator*(const f32 lhs, const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator*(const Vector3f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector3f operator*(const Vector3f lhs,
                                         const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator/(const f32 lhs, const Vector3f& rhs);
    HELIOS_NO_DISCARD Vector3f operator/(const Vector3f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector3f operator/(const Vector3f lhs,
                                         const Vector3f& rhs);
    HELIOS_NO_DISCARD constexpr Vector3f abs(const Vector3f& vec) noexcept;
    HELIOS_NO_DISCARD f32 angle(const Vector3f& lhs,
                                const Vector3f& rhs) noexcept;
    HELIOS_NO_DISCARD Vector3f cross(const Vector3f lhs,
                                     const Vector3f& rhs) noexcept;
    HELIOS_NO_DISCARD f32 dot(const Vector3f& lhs,
                              const Vector3f& rhs) noexcept;
    HELIOS_NO_DISCARD f32 euclidianNorm(const Vector3f& vec) noexcept;
    HELIOS_NO_DISCARD f32 length(const Vector3f& vec) noexcept;
    HELIOS_NO_DISCARD f32 magnitude(const Vector3f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm1(const Vector3f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm2(const Vector3f& vec) noexcept;
    HELIOS_NO_DISCARD Vector3f reflect(const Vector3f vec,
                                       const Vector3f& line) noexcept;

    struct alignas(16) Vector4f
    {
        union
        {
            struct
            {
                f32 x;
                f32 y;
                f32 z;
                f32 w;
            };
            struct
            {
                f32 r;
                f32 g;
                f32 b;
                f32 a;
            };
            struct
            {
                f32 s;
                f32 t;
                f32 p;
                f32 q;
            };
            f32 data[4];
        };

        constexpr Vector4f() noexcept;
        constexpr explicit Vector4f(const f32 scalar) noexcept;
        constexpr Vector4f(const f32 x, const f32 y, const f32 z,
                           const f32 w) noexcept;
        constexpr Vector4f(const Vector2f& xy, const f32 z = 0.0f,
                           const f32 w = 0.0f);
        constexpr Vector4f(const Vector2f& xy, const Vector2f& zw);
        constexpr Vector4f(const Vector3f& xyz, const f32 w = 0.0f);
        constexpr Vector4f(const Vector4f& other) noexcept;
        constexpr Vector4f(Vector2f&& xy, Vector2f&& zw);
        constexpr Vector4f(Vector2f&& xy, const f32 z = 0.0f,
                           const f32 w = 0.0f);
        constexpr Vector4f(Vector3f&& xyz, const f32 w = 0.0f);
        constexpr Vector4f(Vector4f&& other) noexcept;
        ~Vector4f() = default;
        constexpr Vector4f& operator=(const f32 rhs) noexcept;
        constexpr Vector4f& operator=(const Vector2f& rhs) noexcept;
        constexpr Vector4f& operator=(const Vector3f& rhs) noexcept;
        constexpr Vector4f& operator=(const Vector4f& rhs) noexcept;
        constexpr Vector4f& operator=(Vector2f&& rhs) noexcept;
        constexpr Vector4f& operator=(Vector3f&& rhs) noexcept;
        constexpr Vector4f& operator=(Vector4f&& rhs) noexcept;
        constexpr bool operator==(const f32 rhs) const noexcept;
        constexpr bool operator==(const Vector4f& rhs) const noexcept;
        constexpr bool operator!=(const f32 rhs) const noexcept;
        constexpr bool operator!=(const Vector4f& rhs) const noexcept;
        Vector4f& operator+=(const f32 rhs) noexcept;
        Vector4f& operator+=(const Vector4f& rhs) noexcept;
        Vector4f& operator-=(const f32 rhs) noexcept;
        Vector4f& operator-=(const Vector4f& rhs) noexcept;
        Vector4f& operator*=(const f32 rhs) noexcept;
        Vector4f& operator*=(const Vector4f& rhs) noexcept;
        Vector4f& operator/=(const f32 rhs) noexcept;
        Vector4f& operator/=(const Vector4f& rhs) noexcept;
        HELIOS_NO_DISCARD constexpr Vector4f abs() const noexcept;
        HELIOS_NO_DISCARD f32 angle(const Vector4f& other) const noexcept;
        HELIOS_NO_DISCARD f32 dot(const Vector4f& other) const noexcept;
        HELIOS_NO_DISCARD f32 euclidianNorm() const noexcept;
        HELIOS_NO_DISCARD f32 length() const noexcept;
        HELIOS_NO_DISCARD f32 magnitude() const noexcept;
        HELIOS_NO_DISCARD f32 norm1() const noexcept;
        HELIOS_NO_DISCARD f32 norm2() const noexcept;
        HELIOS_NO_DISCARD Vector4f reflect(const Vector4f& line) const noexcept;
    };

    constexpr bool operator==(const f32 lhs, const Vector4f& rhs);
    constexpr bool operator!=(const f32 lhs, const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator+(const f32 lhs, const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator+(const Vector4f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector4f operator+(const Vector4f lhs,
                                         const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator-(const f32 lhs, const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator-(const Vector4f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector4f operator-(const Vector4f lhs,
                                         const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator*(const f32 lhs, const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator*(const Vector4f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector4f operator*(const Vector4f lhs,
                                         const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator/(const f32 lhs, const Vector4f& rhs);
    HELIOS_NO_DISCARD Vector4f operator/(const Vector4f lhs, const f32 rhs);
    HELIOS_NO_DISCARD Vector4f operator/(const Vector4f lhs,
                                         const Vector4f& rhs);
    HELIOS_NO_DISCARD constexpr Vector4f abs(const Vector4f& vec) noexcept;
    HELIOS_NO_DISCARD f32 angle(const Vector4f& lhs,
                                const Vector4f& rhs) noexcept;
    HELIOS_NO_DISCARD f32 dot(const Vector4f& lhs,
                              const Vector4f& rhs) noexcept;
    HELIOS_NO_DISCARD f32 euclidianNorm(const Vector4f& vec) noexcept;
    HELIOS_NO_DISCARD f32 length(const Vector4f& vec) noexcept;
    HELIOS_NO_DISCARD f32 magnitude(const Vector4f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm1(const Vector4f& vec) noexcept;
    HELIOS_NO_DISCARD f32 norm2(const Vector4f& vec) noexcept;
    HELIOS_NO_DISCARD Vector4f reflect(const Vector4f vec,
                                       const Vector4f& line) noexcept;

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

    constexpr bool Vector2f::operator==(const f32 rhs) const noexcept
    {
        return data[0] == rhs && data[1] == rhs;
    }

    constexpr bool Vector2f::operator==(const Vector2f& rhs) const noexcept
    {
        return data[0] == rhs.data[0] && data[1] == rhs.data[1];
    }

    constexpr bool Vector2f::operator!=(const f32 rhs) const noexcept
    {
        return data[0] != rhs || data[1] != rhs;
    }

    constexpr bool Vector2f::operator!=(const Vector2f& rhs) const noexcept
    {
        return data[0] != rhs.data[0] || data[1] != rhs.data[1];
    }

    constexpr Vector2f Vector2f::abs() const noexcept
    {
        return Vector2f(helios::abs(data[0]), helios::abs(data[1]));
    }

    constexpr bool operator==(const f32 lhs, const Vector2f& rhs)
    {
        return lhs == rhs.data[0] && lhs == rhs.data[1];
    }

    constexpr bool operator!=(const f32 lhs, const Vector2f& rhs)
    {
        return lhs != rhs.data[0] || lhs != rhs.data[1];
    }

    constexpr Vector2f abs(const Vector2f& vec) noexcept
    {
        return vec.abs();
    }

    constexpr Vector3f::Vector3f() noexcept : Vector3f(0.0f)
    {
    }

    constexpr Vector3f::Vector3f(const f32 scalar) noexcept
        : Vector3f(scalar, scalar, scalar)
    {
    }

    constexpr Vector3f::Vector3f(const f32 x, const f32 y, const f32 z) noexcept
        : data()
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
    }

    constexpr Vector3f::Vector3f(const Vector2f& xy, const f32 z)
        : Vector3f(xy.data[0], xy.data[1], z)
    {
    }

    constexpr Vector3f::Vector3f(const Vector3f& other) noexcept
        : Vector3f(other.data[0], other.data[1], other.data[2])
    {
    }

    constexpr Vector3f::Vector3f(Vector2f&& xy, const f32 z)
        : Vector3f(helios::move(xy.data[0]), helios::move(xy.data[1]), z)
    {
    }

    constexpr Vector3f::Vector3f(Vector3f&& other) noexcept
        : Vector3f(helios::move(other.data[0]), helios::move(other.data[1]),
                   helios::move(other.data[2]))
    {
    }

    constexpr Vector3f& Vector3f::operator=(const f32 rhs) noexcept
    {
        data[0] = data[1] = data[2] = rhs;
        return *this;
    }

    constexpr Vector3f& Vector3f::operator=(const Vector2f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = 0.0f;
        return *this;
    }

    constexpr Vector3f& Vector3f::operator=(const Vector3f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = rhs.data[2];
        return *this;
    }

    constexpr Vector3f& Vector3f::operator=(Vector2f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        data[2] = 0;
        return *this;
    }

    constexpr Vector3f& Vector3f::operator=(Vector3f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        data[2] = helios::move(rhs.data[2]);
        return *this;
    }

    constexpr bool Vector3f::operator==(const f32 rhs) const noexcept
    {
        return data[0] == rhs && data[1] == rhs && data[2] == rhs;
    }

    constexpr bool Vector3f::operator==(const Vector3f& rhs) const noexcept
    {
        return data[0] == rhs.data[0] && data[1] == rhs.data[1] &&
               data[2] == rhs.data[2];
    }

    constexpr bool Vector3f::operator!=(const f32 rhs) const noexcept
    {
        return data[0] != rhs || data[1] != 0.0f || data[2] != 0.0f;
    }

    constexpr bool Vector3f::operator!=(const Vector3f& rhs) const noexcept
    {
        return data[0] != rhs.data[0] || data[1] != rhs.data[1] ||
               data[2] != rhs.data[2];
    }

    constexpr bool operator==(const f32 lhs, const Vector3f& rhs)
    {
        return rhs != lhs;
    }

    constexpr bool operator!=(const f32 lhs, const Vector3f& rhs)
    {
        return rhs != lhs;
    }

    constexpr Vector3f Vector3f::abs() const noexcept
    {
        return Vector3f(helios::abs(data[0]), helios::abs(data[1]),
                        helios::abs(data[2]));
    }

    constexpr Vector3f abs(const Vector3f& vec) noexcept
    {
        return vec.abs();
    }

    constexpr Vector4f::Vector4f() noexcept : Vector4f(0.0f)
    {
    }

    constexpr Vector4f::Vector4f(const f32 scalar) noexcept
        : Vector4f(scalar, scalar, scalar, scalar)
    {
    }

    constexpr Vector4f::Vector4f(const f32 x, const f32 y, const f32 z,
                                 const f32 w) noexcept
        : data()
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
        data[3] = w;
    }

    constexpr Vector4f::Vector4f(const Vector2f& xy, const f32 z, const f32 w)
        : Vector4f(xy.x, xy.y, z, w)
    {
    }

    constexpr Vector4f::Vector4f(const Vector2f& xy, const Vector2f& zw)
        : Vector4f(xy.x, xy.y, zw.x, zw.y)
    {
    }

    constexpr Vector4f::Vector4f(const Vector3f& xyz, const f32 w)
        : Vector4f(xyz.x, xyz.y, xyz.z, w)
    {
    }

    constexpr Vector4f::Vector4f(const Vector4f& other) noexcept
        : Vector4f(other.x, other.y, other.z, other.w)
    {
    }

    constexpr Vector4f::Vector4f(Vector2f&& xy, Vector2f&& zw)
        : Vector4f(helios::move(xy.x), helios::move(xy.y), helios::move(zw.x),
                   helios::move(zw.y))
    {
    }

    constexpr Vector4f::Vector4f(Vector2f&& xy, const f32 z, const f32 w)
        : Vector4f(helios::move(xy.x), helios::move(xy.y), z, w)
    {
    }

    constexpr Vector4f::Vector4f(Vector3f&& xyz, const f32 w)
        : Vector4f(helios::move(xyz.x), helios::move(xyz.y),
                   helios::move(xyz.z), w)
    {
    }

    constexpr Vector4f::Vector4f(Vector4f&& other) noexcept
        : Vector4f(helios::move(other.x), helios::move(other.y),
                   helios::move(other.z), helios::move(other.w))
    {
    }

    constexpr Vector4f& Vector4f::operator=(const f32 rhs) noexcept
    {
        data[0] = data[1] = data[2] = data[3] = rhs;
        return *this;
    }

    constexpr Vector4f& Vector4f::operator=(const Vector2f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = data[3] = 0.0f;
        return *this;
    }

    constexpr Vector4f& Vector4f::operator=(const Vector3f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = rhs.data[2];
        data[3] = 0.0f;
        return *this;
    }

    constexpr Vector4f& Vector4f::operator=(const Vector4f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = rhs.data[2];
        data[3] = rhs.data[3];
        return *this;
    }

    constexpr Vector4f& Vector4f::operator=(Vector2f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        data[2] = data[3] = 0.0f;
        return *this;
    }

    constexpr Vector4f& Vector4f::operator=(Vector3f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        data[2] = helios::move(rhs.data[2]);
        data[3] = 0.0f;
        return *this;
    }

    constexpr Vector4f& Vector4f::operator=(Vector4f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        data[2] = helios::move(rhs.data[2]);
        data[3] = helios::move(rhs.data[3]);
        return *this;
    }

    constexpr bool Vector4f::operator==(const f32 rhs) const noexcept
    {
        return data[0] == rhs && data[1] == rhs && data[2] == rhs &&
               data[3] == rhs;
    }

    constexpr bool Vector4f::operator==(const Vector4f& rhs) const noexcept
    {
        return data[0] == rhs.data[0] && data[1] == rhs.data[1] &&
               data[2] == rhs.data[2] && data[3] == rhs.data[3];
    }

    constexpr bool Vector4f::operator!=(const f32 rhs) const noexcept
    {
        return data[0] != rhs || data[1] != rhs || data[2] != rhs ||
               data[3] != rhs;
    }

    constexpr bool Vector4f::operator!=(const Vector4f& rhs) const noexcept
    {
        return data[0] != rhs.data[0] || data[1] != rhs.data[1] ||
               data[2] != rhs.data[2] || data[3] != rhs.data[3];
    }

    constexpr Vector4f Vector4f::abs() const noexcept
    {
        return Vector4f(helios::abs(data[0]), helios::abs(data[1]),
                        helios::abs(data[2]), helios::abs(data[3]));
    }

    constexpr bool operator==(const f32 lhs, const Vector4f& rhs)
    {
        return rhs == lhs;
    }

    constexpr bool operator!=(const f32 lhs, const Vector4f& rhs)
    {
        return rhs != lhs;
    }

    constexpr Vector4f abs(const Vector4f& vec) noexcept
    {
        return vec.abs();
    }
} // namespace helios
