#include <helios/math/matrix.hpp>

#include <smmintrin.h>
#include <xmmintrin.h>

namespace helios
{
    Matrix4f& Matrix4f::operator+=(const Matrix4f& rhs)
    {
        __m128 lCol0 = _mm_load_ps(data + 0);
        __m128 lCol1 = _mm_load_ps(data + 4);
        __m128 lCol2 = _mm_load_ps(data + 8);
        __m128 lCol3 = _mm_load_ps(data + 12);

        __m128 rCol0 = _mm_load_ps(rhs.data + 0);
        __m128 rCol1 = _mm_load_ps(rhs.data + 4);
        __m128 rCol2 = _mm_load_ps(rhs.data + 8);
        __m128 rCol3 = _mm_load_ps(rhs.data + 12);

        lCol0 = _mm_add_ps(lCol0, rCol0);
        lCol1 = _mm_add_ps(lCol1, rCol1);
        lCol2 = _mm_add_ps(lCol2, rCol2);
        lCol3 = _mm_add_ps(lCol3, rCol3);

        _mm_store_ps(data + 0, lCol0);
        _mm_store_ps(data + 4, lCol1);
        _mm_store_ps(data + 8, lCol2);
        _mm_store_ps(data + 12, lCol3);

        return *this;
    }

    Matrix4f& Matrix4f::operator-=(const Matrix4f& rhs)
    {
        __m128 lCol0 = _mm_load_ps(data + 0);
        __m128 lCol1 = _mm_load_ps(data + 4);
        __m128 lCol2 = _mm_load_ps(data + 8);
        __m128 lCol3 = _mm_load_ps(data + 12);

        __m128 rCol0 = _mm_load_ps(rhs.data + 0);
        __m128 rCol1 = _mm_load_ps(rhs.data + 4);
        __m128 rCol2 = _mm_load_ps(rhs.data + 8);
        __m128 rCol3 = _mm_load_ps(rhs.data + 12);

        lCol0 = _mm_sub_ps(lCol0, rCol0);
        lCol1 = _mm_sub_ps(lCol1, rCol1);
        lCol2 = _mm_sub_ps(lCol2, rCol2);
        lCol3 = _mm_sub_ps(lCol3, rCol3);

        _mm_store_ps(data + 0, lCol0);
        _mm_store_ps(data + 4, lCol1);
        _mm_store_ps(data + 8, lCol2);
        _mm_store_ps(data + 12, lCol3);

        return *this;
    }

    Matrix4f& Matrix4f::operator*=(const f32 scalar)
    {
        __m128 lCol0 = _mm_load_ps(data + 0);
        __m128 lCol1 = _mm_load_ps(data + 4);
        __m128 lCol2 = _mm_load_ps(data + 8);
        __m128 lCol3 = _mm_load_ps(data + 12);

        __m128 multi = _mm_set_ps1(scalar);

        lCol0 = _mm_mul_ps(lCol0, multi);
        lCol1 = _mm_mul_ps(lCol1, multi);
        lCol2 = _mm_mul_ps(lCol2, multi);
        lCol3 = _mm_mul_ps(lCol3, multi);

        _mm_store_ps(data + 0, lCol0);
        _mm_store_ps(data + 4, lCol1);
        _mm_store_ps(data + 8, lCol2);
        _mm_store_ps(data + 12, lCol3);

        return *this;
    }

    Matrix4f& Matrix4f::operator*=(const Matrix4f& rhs)
    {
        f32 res[16];

        __m128 col0 = _mm_load_ps(data + 0);
        __m128 col1 = _mm_load_ps(data + 4);
        __m128 col2 = _mm_load_ps(data + 8);
        __m128 col3 = _mm_load_ps(data + 12);

        for (i32 i = 0; i < 4; i++)
        {
            __m128 element0 = _mm_set_ps1(rhs.data[4 * i + 0]);
            __m128 element1 = _mm_set_ps1(rhs.data[4 * i + 1]);
            __m128 element2 = _mm_set_ps1(rhs.data[4 * i + 2]);
            __m128 element3 = _mm_set_ps1(rhs.data[4 * i + 3]);

            __m128 result = _mm_add_ps(_mm_add_ps(_mm_mul_ps(element0, col0),
                                                  _mm_mul_ps(element1, col1)),
                                       _mm_add_ps(_mm_mul_ps(element2, col2),
                                                  _mm_mul_ps(element3, col3)));
            _mm_store_ps(res + 4 * i, result);
        }

        _mm_store_ps(data + 0, _mm_load_ps(res + 0));
        _mm_store_ps(data + 4, _mm_load_ps(res + 4));
        _mm_store_ps(data + 8, _mm_load_ps(res + 8));
        _mm_store_ps(data + 12, _mm_load_ps(res + 12));

        return *this;
    }

    Matrix4f Matrix4f::inverse() const noexcept
    {
        // based on GLM implementation

        f32 c00 = data[2 * 4 + 2] * data[3 * 4 + 3] -
                  data[3 * 4 + 2] * data[2 * 4 + 3];
        f32 c02 = data[1 * 4 + 2] * data[3 * 4 + 3] -
                  data[3 * 4 + 2] * data[1 * 4 + 3];
        f32 c03 = data[1 * 4 + 2] * data[2 * 4 + 3] -
                  data[2 * 4 + 2] * data[1 * 4 + 3];

        f32 c04 = data[2 * 4 + 1] * data[3 * 4 + 3] -
                  data[3 * 4 + 1] * data[2 * 4 + 3];
        f32 c06 = data[1 * 4 + 1] * data[3 * 4 + 3] -
                  data[3 * 4 + 1] * data[1 * 4 + 3];
        f32 c07 = data[1 * 4 + 1] * data[2 * 4 + 3] -
                  data[2 * 4 + 1] * data[1 * 4 + 3];

        f32 c08 = data[2 * 4 + 1] * data[3 * 4 + 2] -
                  data[3 * 4 + 1] * data[2 * 4 + 2];
        f32 c10 = data[1 * 4 + 1] * data[3 * 4 + 2] -
                  data[3 * 4 + 1] * data[1 * 4 + 2];
        f32 c11 = data[1 * 4 + 1] * data[2 * 4 + 2] -
                  data[2 * 4 + 1] * data[1 * 4 + 2];

        f32 c12 = data[2 * 4 + 0] * data[3 * 4 + 3] -
                  data[3 * 4 + 0] * data[2 * 4 + 3];
        f32 c14 = data[1 * 4 + 0] * data[3 * 4 + 3] -
                  data[3 * 4 + 0] * data[1 * 4 + 3];
        f32 c15 = data[1 * 4 + 0] * data[2 * 4 + 3] -
                  data[2 * 4 + 0] * data[1 * 4 + 3];

        f32 c16 = data[2 * 4 + 0] * data[3 * 4 + 2] -
                  data[3 * 4 + 0] * data[2 * 4 + 2];
        f32 c18 = data[1 * 4 + 0] * data[3 * 4 + 2] -
                  data[3 * 4 + 0] * data[1 * 4 + 2];
        f32 c19 = data[1 * 4 + 0] * data[2 * 4 + 2] -
                  data[2 * 4 + 0] * data[1 * 4 + 2];

        f32 c20 = data[2 * 4 + 0] * data[3 * 4 + 1] -
                  data[3 * 4 + 0] * data[2 * 4 + 1];
        f32 c22 = data[1 * 4 + 0] * data[3 * 4 + 1] -
                  data[3 * 4 + 0] * data[1 * 4 + 1];
        f32 c23 = data[1 * 4 + 0] * data[2 * 4 + 1] -
                  data[2 * 4 + 0] * data[1 * 4 + 1];

        Vector4f fac0(c00, c00, c02, c03);
        Vector4f fac1(c04, c04, c06, c07);
        Vector4f fac2(c08, c08, c10, c11);
        Vector4f fac3(c12, c12, c14, c15);
        Vector4f fac4(c16, c16, c18, c19);
        Vector4f fac5(c20, c20, c22, c23);

        Vector4f vec0(data[1 * 4 + 0], data[0 * 4 + 0], data[0 * 4 + 0],
                      data[0 * 4 + 0]);
        Vector4f vec1(data[1 * 4 + 1], data[0 * 4 + 1], data[0 * 4 + 1],
                      data[0 * 4 + 1]);
        Vector4f vec2(data[1 * 4 + 2], data[0 * 4 + 2], data[0 * 4 + 2],
                      data[0 * 4 + 2]);
        Vector4f vec3(data[1 * 4 + 3], data[0 * 4 + 3], data[0 * 4 + 3],
                      data[0 * 4 + 3]);

        Vector4f inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
        Vector4f inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
        Vector4f inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
        Vector4f inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

        Vector4f sign1(1.0f, -1.0f, 1.0f, -1.0f);
        Vector4f sign2(-1.0f, 1.0f, -1.0f, 1.0f);

        Matrix4f inverse(inv0 * sign1, inv1 * sign2, inv2 * sign1,
                         inv3 * sign2);
        Vector4f row0 = {inverse.data[0], inverse.data[4], inverse.data[8],
                         inverse.data[12]};
        Vector4f dot0 = col0 * row0;
        f32 dot1 = dot0.x + dot0.y + dot0.z + dot0.w;
        f32 inv = 1.0f / dot1;
        return inverse *= inv;
    }

    Matrix4f operator+(const Matrix4f& lhs, const Matrix4f& rhs) noexcept
    {
        Matrix4f res;

        __m128 lCol0 = _mm_load_ps(lhs.data + 0);
        __m128 lCol1 = _mm_load_ps(lhs.data + 4);
        __m128 lCol2 = _mm_load_ps(lhs.data + 8);
        __m128 lCol3 = _mm_load_ps(lhs.data + 12);

        __m128 rCol0 = _mm_load_ps(rhs.data + 0);
        __m128 rCol1 = _mm_load_ps(rhs.data + 4);
        __m128 rCol2 = _mm_load_ps(rhs.data + 8);
        __m128 rCol3 = _mm_load_ps(rhs.data + 12);

        lCol0 = _mm_add_ps(lCol0, rCol0);
        lCol1 = _mm_add_ps(lCol1, rCol1);
        lCol2 = _mm_add_ps(lCol2, rCol2);
        lCol3 = _mm_add_ps(lCol3, rCol3);

        _mm_store_ps(res.data + 0, lCol0);
        _mm_store_ps(res.data + 4, lCol1);
        _mm_store_ps(res.data + 8, lCol2);
        _mm_store_ps(res.data + 12, lCol3);

        return res;
    }

    Matrix4f operator-(const Matrix4f& lhs, const Matrix4f& rhs) noexcept
    {
        Matrix4f res;

        __m128 lCol0 = _mm_load_ps(lhs.data + 0);
        __m128 lCol1 = _mm_load_ps(lhs.data + 4);
        __m128 lCol2 = _mm_load_ps(lhs.data + 8);
        __m128 lCol3 = _mm_load_ps(lhs.data + 12);

        __m128 rCol0 = _mm_load_ps(rhs.data + 0);
        __m128 rCol1 = _mm_load_ps(rhs.data + 4);
        __m128 rCol2 = _mm_load_ps(rhs.data + 8);
        __m128 rCol3 = _mm_load_ps(rhs.data + 12);

        lCol0 = _mm_sub_ps(lCol0, rCol0);
        lCol1 = _mm_sub_ps(lCol1, rCol1);
        lCol2 = _mm_sub_ps(lCol2, rCol2);
        lCol3 = _mm_sub_ps(lCol3, rCol3);

        _mm_store_ps(res.data + 0, lCol0);
        _mm_store_ps(res.data + 4, lCol1);
        _mm_store_ps(res.data + 8, lCol2);
        _mm_store_ps(res.data + 12, lCol3);

        return res;
    }

    Matrix4f operator*(const Matrix4f& lhs, const f32 rhs) noexcept
    {
        Matrix4f res;
        __m128 lCol0 = _mm_load_ps(lhs.data + 0);
        __m128 lCol1 = _mm_load_ps(lhs.data + 4);
        __m128 lCol2 = _mm_load_ps(lhs.data + 8);
        __m128 lCol3 = _mm_load_ps(lhs.data + 12);

        __m128 multi = _mm_set_ps1(rhs);

        lCol0 = _mm_mul_ps(lCol0, multi);
        lCol1 = _mm_mul_ps(lCol1, multi);
        lCol2 = _mm_mul_ps(lCol2, multi);
        lCol3 = _mm_mul_ps(lCol3, multi);

        _mm_store_ps(res.data + 0, lCol0);
        _mm_store_ps(res.data + 4, lCol1);
        _mm_store_ps(res.data + 8, lCol2);
        _mm_store_ps(res.data + 12, lCol3);

        return res;
    }

    Matrix4f operator*(const f32 lhs, const Matrix4f& rhs) noexcept
    {
        Matrix4f res;
        __m128 lCol0 = _mm_load_ps(rhs.data + 0);
        __m128 lCol1 = _mm_load_ps(rhs.data + 4);
        __m128 lCol2 = _mm_load_ps(rhs.data + 8);
        __m128 lCol3 = _mm_load_ps(rhs.data + 12);

        __m128 multi = _mm_set_ps1(lhs);

        lCol0 = _mm_mul_ps(lCol0, multi);
        lCol1 = _mm_mul_ps(lCol1, multi);
        lCol2 = _mm_mul_ps(lCol2, multi);
        lCol3 = _mm_mul_ps(lCol3, multi);

        _mm_store_ps(res.data + 0, lCol0);
        _mm_store_ps(res.data + 4, lCol1);
        _mm_store_ps(res.data + 8, lCol2);
        _mm_store_ps(res.data + 12, lCol3);

        return res;
    }

    Matrix4f operator*(const Matrix4f& lhs, const Matrix4f& rhs) noexcept
    {
        Matrix4f res;

        __m128 col0 = _mm_load_ps(lhs.data + 0);
        __m128 col1 = _mm_load_ps(lhs.data + 4);
        __m128 col2 = _mm_load_ps(lhs.data + 8);
        __m128 col3 = _mm_load_ps(lhs.data + 12);

        for (i32 i = 0; i < 4; i++)
        {
            __m128 element0 = _mm_set_ps1(rhs.data[4 * i + 0]);
            __m128 element1 = _mm_set_ps1(rhs.data[4 * i + 1]);
            __m128 element2 = _mm_set_ps1(rhs.data[4 * i + 2]);
            __m128 element3 = _mm_set_ps1(rhs.data[4 * i + 3]);

            __m128 result = _mm_add_ps(_mm_add_ps(_mm_mul_ps(element0, col0),
                                                  _mm_mul_ps(element1, col1)),
                                       _mm_add_ps(_mm_mul_ps(element2, col2),
                                                  _mm_mul_ps(element3, col3)));
            _mm_store_ps(res.data + 4 * i, result);
        }

        return res;
    }

    Vector4f operator*(const Matrix4f& lhs, const Vector4f& rhs) noexcept
    {
        Vector4f res;

        __m128 x = _mm_set_ps1(rhs.data[0]);
        __m128 y = _mm_set_ps1(rhs.data[1]);
        __m128 z = _mm_set_ps1(rhs.data[2]);
        __m128 w = _mm_set_ps1(rhs.data[3]);

        __m128 c0 = _mm_load_ps(lhs.data + 0);
        __m128 c1 = _mm_load_ps(lhs.data + 4);
        __m128 c2 = _mm_load_ps(lhs.data + 8);
        __m128 c3 = _mm_load_ps(lhs.data + 12);

        _mm_store_ps(
            res.data,
            _mm_add_ps(_mm_add_ps(_mm_mul_ps(x, c0), _mm_mul_ps(y, c1)),
                       _mm_add_ps(_mm_mul_ps(z, c2), _mm_mul_ps(w, c3))));

        return res;
    };

    Matrix4f inverse(const Matrix4f& mat) noexcept
    {
        return mat.inverse();
    }
} // namespace helios
