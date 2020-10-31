#include <helios/core/transformation.hpp>

#include <helios/math/transformations.hpp>

namespace helios
{
    TransformationComponent::TransformationComponent(const Vector3f& pos, const Vector3f& rot,
                                   const Vector3f& sca) noexcept
    {
        _position = pos;
        _rotation = rot;
        _scale = sca;
        _transform = transform(_position, _rotation, _scale);
    }

    Vector3f TransformationComponent::getPosition() const noexcept
    {
        return _position;
    }

    void TransformationComponent::setPosition(const Vector3f& pos) noexcept
    {
        _position = pos;
        _transform = transform(_position, _rotation, _scale);
    }

    Vector3f TransformationComponent::getRotation() const noexcept
    {
        return _rotation;
    }

    void TransformationComponent::setRotation(const Vector3f& rot) noexcept
    {
        _rotation = rot;
        _transform = transform(_position, _rotation, _scale);
    }

    Vector3f TransformationComponent::getScale() const noexcept
    {
        return _scale;
    }

    void TransformationComponent::setScale(const Vector3f& sca) noexcept
    {
        _scale = sca;
        _transform = transform(_position, _rotation, _scale);
    }

    Matrix4f TransformationComponent::getTransform() const noexcept
    {
        return _transform;
    }
} // namespace helios