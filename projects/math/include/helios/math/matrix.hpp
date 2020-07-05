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

        Matrix4f() noexcept;
        Matrix4f(const f32 diagonal) noexcept;
        Matrix4f(const Vector4f& col0, const Vector4f& col1, const Vector4f& col2, const Vector4f& col3) noexcept;
        explicit Matrix4f(const float values[16], const bool aligned = false) noexcept;
        Matrix4f(const Matrix4f& other) noexcept;
        Matrix4f(Matrix4f&& other) noexcept;
        ~Matrix4f() = default;
        Matrix4f& operator=(const f32 diagonal) noexcept;
        Matrix4f& operator=(const Matrix4f& rhs) noexcept;
        Matrix4f& operator=(Matrix4f&& rhs) noexcept;
        bool operator==(const Matrix4f& rhs) const noexcept;
        bool operator!=(const Matrix4f& rhs) const noexcept;

        Matrix4f& operator+=(const Matrix4f& rhs);
        Matrix4f& operator-=(const Matrix4f& rhs);
        Matrix4f& operator*=(const f32 scalar);
        Matrix4f& operator*=(const Matrix4f& rhs);

        HELIOS_NO_DISCARD Matrix4f inverse() const noexcept;
    };

    HELIOS_NO_DISCARD Matrix4f operator+(const Matrix4f& lhs, const Matrix4f& rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator-(const Matrix4f& lhs, const Matrix4f& rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator*(const Matrix4f& lhs, const f32 rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator*(const f32 lhs, const Matrix4f& rhs) noexcept;
    HELIOS_NO_DISCARD Vector4f operator*(const Matrix4f& lhs, const Vector4f& rhs) noexcept;
    HELIOS_NO_DISCARD Matrix4f operator*(const Matrix4f& lhs, const Matrix4f& rhs) noexcept;

    HELIOS_NO_DISCARD Matrix4f inverse(const Matrix4f& mat) noexcept;
} // namespace helios
