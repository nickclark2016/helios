#include <helios/core/keyboard.hpp>

namespace helios
{
    Keyboard::Keyboard()
    {
        for (u32 i = 0; i < KeyCount; i++)
        {
            _status[i] = EKeyStatus::RELEASE;
        }
    }

    bool Keyboard::isPressed(const EKey key) const noexcept
    {
        const u32 k = static_cast<u32>(key);
        return _status[k] == EKeyStatus::PRESS;
    }

    bool Keyboard::isReleased(const EKey key) const noexcept
    {
        const u32 k = static_cast<u32>(key);
        return _status[k] == EKeyStatus::RELEASE;
    }

    bool Keyboard::isHeld(const EKey key) const noexcept
    {
        const u32 k = static_cast<u32>(key);
        return _status[k] == EKeyStatus::REPEAT;
    }

    void Keyboard::setStatus(const EKey key, const EKeyStatus status) noexcept
    {
        const u32 k = static_cast<u32>(key);
        _status[k] = status;
    }
} // namespace helios
