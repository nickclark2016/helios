#pragma once

#include <helios/macros.hpp>
#include <helios/math/vector.hpp>

namespace helios
{
    struct alignas(16) Matrix4f
    {
        union
        {
            struct
            {
                Vector4f col0;
                Vector4f col1;
                Vector4f col2;
                Vector4f col3;
            };
            f32 data[16];
        };

        constexpr Matrix4f() noexcept;
        constexpr Matrix4f(const float diagonal) noexcept;
        constexpr Matrix4f(const Vector4f& col0, const Vector4f& col1,
                           const Vector4f& col2, const Vector4f& col3) noexcept;
        constexpr explicit Matrix4f(const float values[16]) noexcept;
        constexpr Matrix4f(const Matrix4f& other) noexcept;
        constexpr Matrix4f(Matrix4f&& other) noexcept;
        ~Matrix4f() = default;
        constexpr Matrix4f& operator=(const float diagonal) noexcept;
        constexpr Matrix4f& operator=(const Matrix4f& rhs) noexcept;
        constexpr Matrix4f& operator=(Matrix4f&& rhs) noexcept;
        constexpr bool operator==(const Matrix4f& rhs) const noexcept;
        constexpr bool operator!=(const Matrix4f& rhs) const noexcept;

        Matrix4f& operator+=(const Matrix4f& rhs);
        Matrix4f& operator-=(const Matrix4f& rhs);
        Matrix4f& operator*=(const f32 scalar);
        Matrix4f& operator*=(const Matrix4f& rhs);

        HELIOS_NO_DISCARD Matrix4f inverse() const noexcept;
    };

    HELIOS_NO_DISCARD Matrix4f operator+(const Matrix4f& lhs,
                                         const Matrix4f& rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator-(const Matrix4f& lhs,
                                         const Matrix4f& rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator*(const Matrix4f& lhs,
                                         const f32 rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator*(const f32 lhs,
                                         const Matrix4f& rhs) noexcept;
    HELIOS_NO_DISCARD Vector4f operator*(const Matrix4f& lhs,
                                         const Vector4f& rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator*(const Matrix4f& lhs,
                                         const Matrix4f& rhs) noexcept;

    HELIOS_NO_DISCARD Matrix4f inverse(const Matrix4f& mat) noexcept;

    constexpr Matrix4f::Matrix4f() noexcept : Matrix4f(0.0f)
    {
    }

    constexpr Matrix4f::Matrix4f(const float diagonal) noexcept
        : Matrix4f(Vector4f(diagonal, 0.0f, 0.0f, 0.0f),
                   Vector4f(0.0f, diagonal, 0.0f, 0.0f),
                   Vector4f(0.0f, 0.0f, diagonal, 0.0f),
                   Vector4f(0.0f, 0.0f, 0.0f, diagonal))
    {
    }

    constexpr Matrix4f::Matrix4f(const Vector4f& col0, const Vector4f& col1,
                                 const Vector4f& col2,
                                 const Vector4f& col3) noexcept
        : data()
    {
        data[0] = col0.x;
        data[1] = col0.y;
        data[2] = col0.z;
        data[3] = col0.w;
        data[4] = col1.x;
        data[5] = col1.y;
        data[6] = col1.z;
        data[7] = col1.w;
        data[8] = col2.x;
        data[9] = col2.y;
        data[10] = col2.z;
        data[11] = col2.w;
        data[12] = col3.x;
        data[13] = col3.y;
        data[14] = col3.z;
        data[15] = col3.w;
    }

    constexpr Matrix4f::Matrix4f(const float values[16]) noexcept : data()
    {
        data[0] = values[0];
        data[1] = values[1];
        data[2] = values[2];
        data[3] = values[3];
        data[4] = values[4];
        data[5] = values[5];
        data[6] = values[6];
        data[7] = values[7];
        data[8] = values[8];
        data[9] = values[9];
        data[10] = values[10];
        data[11] = values[11];
        data[12] = values[12];
        data[13] = values[13];
        data[14] = values[14];
        data[15] = values[15];
    }

    constexpr Matrix4f::Matrix4f(const Matrix4f& other) noexcept : data()
    {
        data[0] = other.data[0];
        data[1] = other.data[1];
        data[2] = other.data[2];
        data[3] = other.data[3];
        data[4] = other.data[4];
        data[5] = other.data[5];
        data[6] = other.data[6];
        data[7] = other.data[7];
        data[8] = other.data[8];
        data[9] = other.data[9];
        data[10] = other.data[10];
        data[11] = other.data[11];
        data[12] = other.data[12];
        data[13] = other.data[13];
        data[14] = other.data[14];
        data[15] = other.data[15];
    }

    constexpr Matrix4f::Matrix4f(Matrix4f&& other) noexcept : data()
    {
        data[0] = helios::move(other.data[0]);
        data[1] = helios::move(other.data[1]);
        data[2] = helios::move(other.data[2]);
        data[3] = helios::move(other.data[3]);
        data[4] = helios::move(other.data[4]);
        data[5] = helios::move(other.data[5]);
        data[6] = helios::move(other.data[6]);
        data[7] = helios::move(other.data[7]);
        data[8] = helios::move(other.data[8]);
        data[9] = helios::move(other.data[9]);
        data[10] = helios::move(other.data[10]);
        data[11] = helios::move(other.data[11]);
        data[12] = helios::move(other.data[12]);
        data[13] = helios::move(other.data[13]);
        data[14] = helios::move(other.data[14]);
        data[15] = helios::move(other.data[15]);
    }

    constexpr Matrix4f& Matrix4f::operator=(const float diagonal) noexcept
    {
        data[0] = data[5] = data[10] = data[15] = diagonal;
        return *this;
    }

    constexpr Matrix4f& Matrix4f::operator=(const Matrix4f& rhs) noexcept
    {
        data[0] = rhs.data[0];
        data[1] = rhs.data[1];
        data[2] = rhs.data[2];
        data[3] = rhs.data[3];
        data[4] = rhs.data[4];
        data[5] = rhs.data[5];
        data[6] = rhs.data[6];
        data[7] = rhs.data[7];
        data[8] = rhs.data[8];
        data[9] = rhs.data[9];
        data[10] = rhs.data[10];
        data[11] = rhs.data[11];
        data[12] = rhs.data[12];
        data[13] = rhs.data[13];
        data[14] = rhs.data[14];
        data[15] = rhs.data[15];
        return *this;
    }

    constexpr Matrix4f& Matrix4f::operator=(Matrix4f&& rhs) noexcept
    {
        data[0] = helios::move(rhs.data[0]);
        data[1] = helios::move(rhs.data[1]);
        data[2] = helios::move(rhs.data[2]);
        data[3] = helios::move(rhs.data[3]);
        data[4] = helios::move(rhs.data[4]);
        data[5] = helios::move(rhs.data[5]);
        data[6] = helios::move(rhs.data[6]);
        data[7] = helios::move(rhs.data[7]);
        data[8] = helios::move(rhs.data[8]);
        data[9] = helios::move(rhs.data[9]);
        data[10] = helios::move(rhs.data[10]);
        data[11] = helios::move(rhs.data[11]);
        data[12] = helios::move(rhs.data[12]);
        data[13] = helios::move(rhs.data[13]);
        data[14] = helios::move(rhs.data[14]);
        data[15] = helios::move(rhs.data[15]);
        return *this;
    }

    constexpr bool Matrix4f::operator==(const Matrix4f& rhs) const noexcept
    {
        return data[0] == rhs.data[0] && data[1] == rhs.data[1] &&
               data[2] == rhs.data[2] && data[3] == rhs.data[3] &&
               data[4] == rhs.data[4] && data[5] == rhs.data[5] &&
               data[6] == rhs.data[6] && data[7] == rhs.data[7] &&
               data[8] == rhs.data[8] && data[9] == rhs.data[9] &&
               data[10] == rhs.data[10] && data[11] == rhs.data[11] &&
               data[12] == rhs.data[12] && data[13] == rhs.data[13] &&
               data[14] == rhs.data[14] && data[15] == rhs.data[15];
    }

    constexpr bool Matrix4f::operator!=(const Matrix4f& rhs) const noexcept
    {
        return data[0] != rhs.data[0] || data[1] != rhs.data[1] ||
               data[2] != rhs.data[2] || data[3] != rhs.data[3] ||
               data[4] != rhs.data[4] || data[5] != rhs.data[5] ||
               data[6] != rhs.data[6] || data[7] != rhs.data[7] ||
               data[8] != rhs.data[8] || data[9] != rhs.data[9] ||
               data[10] != rhs.data[10] || data[11] != rhs.data[11] ||
               data[12] != rhs.data[12] || data[13] != rhs.data[13] ||
               data[14] != rhs.data[14] || data[15] != rhs.data[15];
    }
} // namespace helios
