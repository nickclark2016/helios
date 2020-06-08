#pragma once

#include <helios/macros.hpp>

namespace helios
{
    constexpr f32 half_pi = 1.57079632679;
    constexpr f32 pi = 3.14159265359;

    constexpr f32 abs(const f32 num);
    f32 pow(const f32 base, const f32 exp);
    constexpr f32 radians(const f32 degrees);
    constexpr u32 sign(const f32 num);

    // implementation
    inline constexpr f32 abs(const f32 num)
    {
        union fi32
        {
            f32 f;
            u32 i;
        };

        fi32 u = {num};
        u.i &= ~(1UL << 31);
        return u.f;
    }

    inline f32 pow(const f32 base, const f32 exp)
    {
        return __builtin_powf(base, exp);
    }

    inline constexpr f32 radians(const f32 degrees)
    {
        return degrees * pi / 180;
    }

    inline constexpr u32 sign(const f32 num)
    {
        return num < 0.0f ? -1 : (num > 0.0f ? 1 : 0);
    }

} // namespace helios
