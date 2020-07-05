#include <helios/core/mouse.hpp>

namespace helios
{
    Mouse::Mouse()
    {
        for (u32 i = 0; i < ButtonCount; i++)
        {
            _status[i] = EMouseButtonStatus::RELEASE;
        }
    }

    bool Mouse::isPressed(const EMouseButton button) const noexcept
    {
        const u32 btn = static_cast<u32>(button);
        return _status[btn] == EMouseButtonStatus::PRESS;
    }

    bool Mouse::isReleased(const EMouseButton button) const noexcept
    {
        const u32 btn = static_cast<u32>(button);
        return _status[btn] == EMouseButtonStatus::RELEASE;
    }

    f32 Mouse::getX() const noexcept
    {
        return _x;
    }

    f32 Mouse::getY() const noexcept
    {
        return _y;
    }

    f32 Mouse::getDeltaX() const noexcept
    {
        return _dx;
    }

    f32 Mouse::getDeltaY() const noexcept
    {
        return _dy;
    }

    f32 Mouse::getScroll() const noexcept
    {
        return _scroll;
    }

    void Mouse::setStatus(const EMouseButton button, const EMouseButtonStatus status) noexcept
    {
        const u32 btn = static_cast<u32>(button);
        _status[btn] = status;
    }

    void Mouse::setCursor(const f32 x, const f32 y) noexcept
    {
        _dx = x - _x;
        _dy = y - _y;
        _x = x;
        _y = y;
    }

    void Mouse::setScroll(const f32 scroll) noexcept
    {
        _scroll = scroll;
    }
} // namespace helios
