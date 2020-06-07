#pragma once

#include <helios/macros.hpp>

namespace helios
{
    constexpr f32 abs(const f32 num);
    f32 pow(const f32 base, const f32 exp);
    constexpr u32 sign(const f32 num);

    // implementation
    constexpr f32 abs(const f32 num)
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

    f32 pow(const f32 base, const f32 exp)
    {
        return __builtin_powf(base, exp);
    }

    constexpr u32 sign(const f32 num)
    {
        return num < 0.0f ? -1 : (num > 0.0f ? 1 : 0);
    }

} // namespace helios
