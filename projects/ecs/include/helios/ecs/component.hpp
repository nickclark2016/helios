#pragma once

#include <helios/macros.hpp>

namespace helios
{
    struct BaseComponent
    {
        static u32 ID;
    };

    template <typename Type>
    struct Component
    {
    };

    template <typename Type>
    u32 GetComponentID()
    {
        static u32 family = BaseComponent::ID++;
        return family;
    }
} // namespace helios
