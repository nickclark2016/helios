#pragma once

#include <helios/core/property.hpp>
#include <helios/math/matrix.hpp>
#include <helios/math/vector.hpp>

namespace helios
{
    class alignas(64) TransformationComponent
    {
    public:
        TransformationComponent() noexcept = default;
        TransformationComponent(const Vector3f& pos, const Vector3f& rot,
                       const Vector3f& sca) noexcept;

        Vector3f getPosition() const noexcept;
        void setPosition(const Vector3f& pos) noexcept;
        Vector3f getRotation() const noexcept;
        void setRotation(const Vector3f& rot) noexcept;
        Vector3f getScale() const noexcept;
        void setScale(const Vector3f& sca) noexcept;
        Matrix4f getTransform() const noexcept;

        PROPERTY(Vector3f, position, getPosition, setPosition);
        PROPERTY(Vector3f, rotation, getRotation, setRotation);
        PROPERTY(Vector3f, scale, getScale, setScale);
        PROPERTY_READONLY(Matrix4f, matrix, getTransform);

    private:
        Vector3f _position;
        Vector3f _rotation;
        Vector3f _scale;
        Matrix4f _transform;
    };
} // namespace helios