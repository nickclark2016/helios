#pragma once

#include <helios/macros.hpp>

namespace helios
{
    enum class EMouseButtonStatus : u32
    {
        RELEASE = 0,
        PRESS = 1,
    };

    enum class EMouseButton : u32
    {
        BUTTON_1 = 0,
        BUTTON_2 = 1,
        BUTTON_3 = 2,
        BUTTON_4 = 3,
        BUTTON_5 = 4,
        BUTTON_6 = 5,
        BUTTON_7 = 6,
        BUTTON_8 = 7,
        BUTTON_LEFT = BUTTON_1,
        BUTTON_RIGHT = BUTTON_2,
        BUTTON_MIDDLE = BUTTON_3
    };

    class Mouse
    {
    public:
        Mouse();
        ~Mouse() = default;
        HELIOS_NO_COPY_MOVE(Mouse);

        bool isPressed(const EMouseButton button) const noexcept;
        bool isReleased(const EMouseButton button) const noexcept;

        f32 getX() const noexcept;
        f32 getY() const noexcept;
        f32 getDeltaX() const noexcept;
        f32 getDeltaY() const noexcept;
        f32 getScroll() const noexcept;

        void setStatus(const EMouseButton button,
                       const EMouseButtonStatus status) noexcept;
        void setCursor(const f32 x, const f32 y) noexcept;
        void setScroll(const f32 scroll) noexcept;

    private:
        static constexpr u32 ButtonCount =
            static_cast<u32>(EMouseButton::BUTTON_MIDDLE) + 1;
        EMouseButtonStatus _status[ButtonCount];
        f32 _x;
        f32 _y;
        f32 _dx;
        f32 _dy;
        f32 _scroll;
    };
} // namespace helios
