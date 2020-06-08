#include <helios/math/vector.hpp>

namespace helios
{
    Vector2f& Vector2f::operator+=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 res = _mm_add_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator+=(const Vector2f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 res = _mm_add_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator-=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 res = _mm_sub_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator-=(const Vector2f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 res = _mm_sub_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator*=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 res = _mm_mul_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator*=(const Vector2f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 res = _mm_mul_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator/=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 res = _mm_div_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    Vector2f& Vector2f::operator/=(const Vector2f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 res = _mm_div_ps(me, other);
        _mm_storeu_ps(data, res);
        return *this;
    }

    f32 Vector2f::angle(const Vector2f& other) const noexcept
    {
        const f32 productAbs = norm2() * other.norm2();
        const f32 dotProduct = dot(other);
        return cosf(dotProduct / productAbs);
    }

    f32 Vector2f::euclidianNorm() const noexcept
    {
        return norm2();
    }

    f32 Vector2f::length() const noexcept
    {
        return norm2();
    }

    f32 Vector2f::magnitude() const noexcept
    {
        return norm2();
    }

    f32 Vector2f::norm2() const noexcept
    {
        return sqrtf(norm1());
    }

    Vector2f Vector2f::reflect(const Vector2f& line) const noexcept
    {
        // 2 * (v dot l) / (l dot l) - v
        const auto numer = dot(line);
        const auto denom = line.norm1();
        const auto res = 2 * numer / denom * line - *this;
        return res;
    }

    Vector2f operator+(const f32 lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_add_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator+(const Vector2f lhs, const f32 rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_add_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator+(const Vector2f lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_add_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator-(const f32 lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_sub_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator-(const Vector2f lhs, const f32 rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_sub_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator-(const Vector2f lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_sub_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator*(const f32 lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_mul_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator*(const Vector2f lhs, const f32 rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_mul_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator*(const Vector2f lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_mul_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator/(const f32 lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_div_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator/(const Vector2f lhs, const f32 rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_div_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    Vector2f operator/(const Vector2f lhs, const Vector2f& rhs)
    {
        Vector2f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_div_ps(left, right);
        _mm_storeu_ps(res.data, sum);
        return res;
    }

    f32 angle(const Vector2f& lhs, const Vector2f& rhs) noexcept
    {
        return lhs.angle(rhs);
    }

    f32 euclidianNorm(const Vector2f& vec) noexcept
    {
        return vec.euclidianNorm();
    }

    f32 length(const Vector2f& vec) noexcept
    {
        return vec.length();
    }

    f32 magnitude(const Vector2f& vec) noexcept
    {
        return vec.magnitude();
    }

    f32 norm2(const Vector2f& vec) noexcept
    {
        return vec.norm2();
    }

    Vector2f reflect(const Vector2f vec, const Vector2f& line) noexcept
    {
        return vec.reflect(line);
    }
} // namespace helios
