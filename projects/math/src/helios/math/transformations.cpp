#include <helios/math/transformations.hpp>

#include <helios/math/utils.hpp>

namespace helios
{
    Matrix4f translate(const Vector3f& translate)
    {
        Matrix4f res(1.0f);
        res.col3 = Vector4f(translate, 1.0f);
        return res;
    }

    Matrix4f scale(const Vector3f& scale)
    {
        Matrix4f res(1.0f);
        res.data[0] = scale.x;
        res.data[5] = scale.y;
        res.data[10] = scale.z;
        return res;
    }

    Matrix4f rotate(const Matrix4f& src, const Vector3f& axis,
                    const f32 degrees)
    {
        const f32 radians = to_radians(degrees);
        const f32 c = cosf(radians);
        const f32 s = sinf(radians);
        const f32 omc = 1.0f - c;

        const f32 xy = axis.x * axis.y;
        const f32 yz = axis.y * axis.z;
        const f32 xz = axis.x * axis.z;

        const f32 xs = axis.x * s;
        const f32 ys = axis.y * s;
        const f32 zs = axis.z * s;

        const f32 f00 = axis.x * axis.x * omc + c;
        const f32 f01 = xy * omc + zs;
        const f32 f02 = xz * omc - ys;
        const f32 f10 = xy * omc - zs;
        const f32 f11 = axis.y * axis.y * omc + c;
        const f32 f12 = yz * omc + xs;
        const f32 f20 = xz * omc + ys;
        const f32 f21 = yz * omc - xs;
        const f32 f22 = axis.z * axis.z * omc + c;

        const f32 t00 = src.col0.x * f00 + src.col1.x * f01 + src.col2.x * f02;
        const f32 t01 = src.col0.y * f00 + src.col1.y * f01 + src.col2.y * f02;
        const f32 t02 = src.col0.z * f00 + src.col1.z * f01 + src.col2.z * f02;
        const f32 t03 = src.col0.w * f00 + src.col2.w * f01 + src.col2.w * f02;
        const f32 t10 = src.col0.x * f10 + src.col1.x * f11 + src.col2.x * f12;
        const f32 t11 = src.col0.y * f10 + src.col1.y * f11 + src.col2.y * f12;
        const f32 t12 = src.col0.z * f10 + src.col1.z * f11 + src.col2.z * f12;
        const f32 t13 = src.col0.w * f10 + src.col1.w * f11 + src.col2.w * f12;

        Matrix4f res(1.0f);

        res.col2.x = src.col0.x * f20 + src.col1.x * f21 + src.col2.x * f22;
        res.col2.y = src.col0.y * f20 + src.col1.y * f21 + src.col2.y * f22;
        res.col2.z = src.col0.z * f20 + src.col1.z * f21 + src.col2.z * f22;
        res.col2.w = src.col0.w * f20 + src.col1.w * f21 + src.col2.w * f22;
        res.col0.x = t00;
        res.col0.y = t01;
        res.col0.z = t02;
        res.col0.w = t03;
        res.col1.x = t10;
        res.col1.y = t11;
        res.col1.z = t12;
        res.col1.w = t13;

        return res;
    }

    Matrix4f rotate(const Vector3f& eulerAnglesDegrees)
    {
        Matrix4f res(1.0f);
        res = rotate(res, Vector3f(1.0f, 0.0f, 0.0f), eulerAnglesDegrees.x);
        res = rotate(res, Vector3f(0.0f, 1.0f, 0.0f), eulerAnglesDegrees.y);
        res = rotate(res, Vector3f(0.0f, 0.0f, 1.0f), eulerAnglesDegrees.z);
        return res;
    }

    Matrix4f transform(const Vector3f& translation,
                       const Vector3f& rotationEuler, const Vector3f& scalar)
    {
        return translate(translation) * rotate(rotationEuler) * scale(scalar);
    }
} // namespace helios
