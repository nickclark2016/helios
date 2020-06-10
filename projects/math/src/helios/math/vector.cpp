#include <helios/math/vector.hpp>

#include <smmintrin.h>
#include <xmmintrin.h>

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
        return acosf(dotProduct / productAbs);
    }

    f32 Vector2f::dot(const Vector2f& other) const noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 ot = _mm_load_ps(other.data);
        __m128 product = _mm_mul_ps(me, ot);
        __m128 dp = _mm_hadd_ps(product, product);
        int res = _mm_extract_epi32(dp, 0);
        return *reinterpret_cast<float*>(&(res));
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

    f32 Vector2f::norm1() const noexcept
    {
        return dot(*this);
    }

    f32 Vector2f::norm2() const noexcept
    {
        return sqrtf(norm1());
    }

    Vector2f Vector2f::reflect(const Vector2f& line) const noexcept
    {
        // v - 2 * (v dot l) * l
        Vector2f v;
        __m128 dp = _mm_set_ps1(2 * dot(line));
        __m128 vec = _mm_load_ps(data);
        __m128 nor = _mm_load_ps(line.data);
        __m128 res = _mm_sub_ps(vec, _mm_mul_ps(dp, nor));
        _mm_storeu_ps(v.data, res);
        return v;
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

    f32 dot(const Vector2f& lhs, const Vector2f& rhs) noexcept
    {
        return lhs.dot(rhs);
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

    f32 norm1(const Vector2f& vec) noexcept
    {
        return vec.norm1();
    }

    f32 norm2(const Vector2f& vec) noexcept
    {
        return vec.norm2();
    }

    Vector2f reflect(const Vector2f vec, const Vector2f& line) noexcept
    {
        return vec.reflect(line);
    }

    Vector3f& Vector3f::operator+=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 sum = _mm_add_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator+=(const Vector3f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 sum = _mm_add_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator-=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 sum = _mm_sub_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator-=(const Vector3f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 sum = _mm_sub_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator*=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 sum = _mm_mul_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator*=(const Vector3f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 sum = _mm_mul_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator/=(const f32 rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_set_ps1(rhs);
        __m128 sum = _mm_div_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    Vector3f& Vector3f::operator/=(const Vector3f& rhs) noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 other = _mm_load_ps(rhs.data);
        __m128 sum = _mm_div_ps(me, other);
        _mm_store_ps(data, sum);
        return *this;
    }

    f32 Vector3f::angle(const Vector3f& other) const noexcept
    {
        const f32 productAbs = norm2() * other.norm2();
        const f32 dotProduct = dot(other);
        return acosf(dotProduct / productAbs);
    }

    Vector3f Vector3f::cross(const Vector3f& other) const noexcept
    {
        // i  j  k
        // i1 j1 k1
        // i2 j2 k2

        __m128 me = _mm_load_ps(data);
        __m128 oth = _mm_load_ps(other.data);
        __m128 tmp0 = _mm_shuffle_ps(oth, oth, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 tmp1 = _mm_shuffle_ps(me, me, _MM_SHUFFLE(3, 0, 2, 1));
        tmp0 = _mm_mul_ps(tmp0, me);
        tmp1 = _mm_mul_ps(tmp1, oth);
        __m128 tmp2 = _mm_sub_ps(tmp0, tmp1);
        __m128 cp = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));

        Vector3f res;
        _mm_store_ps(res.data, cp);
        return res;
    }

    f32 Vector3f::dot(const Vector3f& other) const noexcept
    {
        __m128 me = _mm_load_ps(data);
        __m128 ot = _mm_load_ps(other.data);
        __m128 product = _mm_mul_ps(me, ot);
        __m128 dp = _mm_hadd_ps(product, product);
        int res = _mm_extract_epi32(_mm_hadd_ps(dp, dp), 0);
        return *reinterpret_cast<float*>(&(res));
    }

    f32 Vector3f::euclidianNorm() const noexcept
    {
        return norm2();
    }

    f32 Vector3f::length() const noexcept
    {
        return norm2();
    }

    f32 Vector3f::magnitude() const noexcept
    {
        return norm2();
    }

    f32 Vector3f::norm1() const noexcept
    {
        return dot(*this);
    }

    f32 Vector3f::norm2() const noexcept
    {
        return sqrtf(norm1());
    }

    Vector3f Vector3f::reflect(const Vector3f& line) const noexcept
    {
        // v - 2 * (v dot l) * l
        Vector3f v;
        __m128 dp = _mm_set_ps1(2 * dot(line));
        __m128 vec = _mm_load_ps(data);
        __m128 nor = _mm_load_ps(line.data);
        __m128 res = _mm_sub_ps(vec, _mm_mul_ps(dp, nor));
        _mm_storeu_ps(v.data, res);
        return v;
    }

    Vector3f operator+(const f32 lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_add_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator+(const Vector3f lhs, const f32 rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_add_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator+(const Vector3f lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_add_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator-(const f32 lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_sub_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator-(const Vector3f lhs, const f32 rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_sub_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator-(const Vector3f lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_sub_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator*(const f32 lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_mul_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator*(const Vector3f lhs, const f32 rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_mul_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator*(const Vector3f lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_mul_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator/(const f32 lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_set_ps1(lhs);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_div_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator/(const Vector3f lhs, const f32 rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_set_ps1(rhs);
        __m128 sum = _mm_div_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    Vector3f operator/(const Vector3f lhs, const Vector3f& rhs)
    {
        Vector3f res;
        __m128 left = _mm_load_ps(lhs.data);
        __m128 right = _mm_load_ps(rhs.data);
        __m128 sum = _mm_div_ps(left, right);
        _mm_store_ps(res.data, sum);
        return res;
    }

    f32 angle(const Vector3f& lhs, const Vector3f& rhs) noexcept
    {
        return lhs.angle(rhs);
    }

    Vector3f cross(const Vector3f lhs, const Vector3f& rhs) noexcept
    {
        return lhs.cross(rhs);
    }

    f32 dot(const Vector3f& lhs, const Vector3f& rhs) noexcept
    {
        return lhs.dot(rhs);
    }

    f32 euclidianNorm(const Vector3f& vec) noexcept
    {
        return vec.euclidianNorm();
    }

    f32 length(const Vector3f& vec) noexcept
    {
        return vec.length();
    }

    f32 magnitude(const Vector3f& vec) noexcept
    {
        return vec.magnitude();
    }

    f32 norm1(const Vector3f& vec) noexcept
    {
        return vec.norm1();
    }

    f32 norm2(const Vector3f& vec) noexcept
    {
        return vec.norm2();
    }

    Vector3f reflect(const Vector3f vec, const Vector3f& line) noexcept
    {
        return vec.reflect(line);
    }
} // namespace helios
