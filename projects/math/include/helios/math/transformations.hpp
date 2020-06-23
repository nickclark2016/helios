#pragma once

#include <helios/math/matrix.hpp>
#include <helios/math/vector.hpp>

namespace helios
{
    Matrix4f translate(const Vector3f& translate);
    Matrix4f scale(const Vector3f& scale);
    Matrix4f rotate(const Matrix4f& src, const Vector3f& axis,
                    const f32 degrees);
    Matrix4f rotate(const Vector3f& eulerAnglesDegrees);
    Matrix4f transform(const Vector3f& translation,
                       const Vector3f& rotationEuler, const Vector3f& scalar);
    Matrix4f orthographic(const f32 left, const f32 right, const f32 bottom,
                          const f32 top, const f32 near, const f32 far);
    Matrix4f perspective(const f32 fov, const f32 aspect, const f32 near,
                         const f32 far);
} // namespace helios
