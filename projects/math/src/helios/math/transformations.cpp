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

    Matrix4f rotate(const Matrix4f& src, const Vector3f& axis, const f32 degrees)
    {
        const f32 radians = to_radians(degrees);
        const f32 c = cosf(radians);
        const f32 s = sinf(radians);
        const f32 oneminusc = 1.0f - c;
        const f32 xy = axis.x * axis.y;
        const f32 yz = axis.y * axis.z;
        const f32 xz = axis.x * axis.z;
        const f32 xs = axis.x * s;
        const f32 ys = axis.y * s;
        const f32 zs = axis.z * s;

        const f32 f00 = axis.x * axis.x * oneminusc + c;
        const f32 f01 = xy * oneminusc + zs;
        const f32 f02 = xz * oneminusc - ys;
        const f32 f10 = xy * oneminusc - zs;
        const f32 f11 = axis.y * axis.y * oneminusc + c;
        const f32 f12 = yz * oneminusc + xs;
        const f32 f20 = xz * oneminusc + ys;
        const f32 f21 = yz * oneminusc - xs;
        const f32 f22 = axis.z * axis.z * oneminusc + c;

        Matrix4f res(1.0f);

        const f32 t00 = src.data[0] * f00 + src.data[4] * f01 + src.data[8] * f02;
        const f32 t01 = src.data[1] * f00 + src.data[5] * f01 + src.data[9] * f02;
        const f32 t02 = src.data[2] * f00 + src.data[6] * f01 + src.data[10] * f02;
        const f32 t03 = src.data[3] * f00 + src.data[7] * f01 + src.data[11] * f02;
        const f32 t10 = src.data[0] * f10 + src.data[4] * f11 + src.data[8] * f12;
        const f32 t11 = src.data[1] * f10 + src.data[5] * f11 + src.data[9] * f12;
        const f32 t12 = src.data[2] * f10 + src.data[6] * f11 + src.data[10] * f12;
        const f32 t13 = src.data[3] * f10 + src.data[7] * f11 + src.data[11] * f12;

        res.data[8] = src.data[0] * f20 + src.data[4] * f21 + src.data[8] * f22;
        res.data[9] = src.data[1] * f20 + src.data[5] * f21 + src.data[9] * f22;
        res.data[10] = src.data[2] * f20 + src.data[6] * f21 + src.data[10] * f22;
        res.data[11] = src.data[3] * f20 + src.data[7] * f21 + src.data[11] * f22;
        res.data[0] = t00;
        res.data[1] = t01;
        res.data[2] = t02;
        res.data[3] = t03;
        res.data[4] = t10;
        res.data[5] = t11;
        res.data[6] = t12;
        res.data[7] = t13;
        return res;
    }

    Matrix4f rotate(const Vector3f& eulerAnglesDegrees)
    {
        auto x = rotate(Matrix4f(1.0f), Vector3f(1.0f, 0.0f, 0.0f), eulerAnglesDegrees.x);
        auto y = rotate(Matrix4f(1.0f), Vector3f(0.0f, 1.0f, 0.0f), eulerAnglesDegrees.y);
        auto z = rotate(Matrix4f(1.0f), Vector3f(0.0f, 0.0f, 1.0f), eulerAnglesDegrees.z);
        return x * y * z;
    }

    Matrix4f transform(const Vector3f& translation, const Vector3f& rotationEuler, const Vector3f& scalar)
    {
        return translate(translation) * rotate(rotationEuler) * scale(scalar);
    }

    Matrix4f orthographic(const f32 left, const f32 right, const f32 bottom, const f32 top, const f32 near,
                          const f32 far)
    {
        Matrix4f res(1.0f);
        res.data[0] = 2 / (right - left);
        res.data[5] = 2 / (top - bottom);
        res.data[10] = -2 / (far - near);
        res.data[12] = -(right + left) / (right - left);
        res.data[13] = -(top + bottom) / (top - bottom);
        res.data[14] = -(far + near) / (far - near);

        return res;
    }

    Matrix4f perspective(const f32 fov, const f32 aspect, const f32 near, const f32 far)
    {
        Matrix4f res(1.0f);
        const f32 yScale = 1.0f / tanf(to_radians(fov / 2.0f));
        const f32 xScale = yScale / aspect;
        const f32 frustum = far - near;

        res.data[0] = xScale;
        res.data[5] = yScale;
        res.data[10] = (-(far + near) / frustum);
        res.data[11] = -1.0f;
        res.data[14] = -((2 * near * far) / frustum);
        res.data[15] = 0.0f;

        return res;
    }
} // namespace helios
