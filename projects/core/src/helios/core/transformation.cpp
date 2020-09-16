#include <helios/core/transformation.hpp>

#include <helios/math/transformations.hpp>

namespace helios
{
    Transformation::Transformation(const Vector3f& pos, const Vector3f& rot,
                                   const Vector3f& sca) noexcept
    {
        _position = pos;
        _rotation = rot;
        _scale = sca;
        _transform = transform(_position, _rotation, _scale);
    }

    Vector3f Transformation::getPosition() const noexcept
    {
        return _position;
    }

    void Transformation::setPosition(const Vector3f& pos) noexcept
    {
        _position = pos;
        _transform = transform(_position, _rotation, _scale);
    }

    Vector3f Transformation::getRotation() const noexcept
    {
        return _rotation;
    }

    void Transformation::setRotation(const Vector3f& rot) noexcept
    {
        _rotation = rot;
        _transform = transform(_position, _rotation, _scale);
    }

    Vector3f Transformation::getScale() const noexcept
    {
        return _scale;
    }

    void Transformation::setScale(const Vector3f& sca) noexcept
    {
        _scale = sca;
        _transform = transform(_position, _rotation, _scale);
    }

    Matrix4f Transformation::getTransform() const noexcept
    {
        return _transform;
    }
} // namespace helios