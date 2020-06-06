#pragma once

#include <helios/render/graphics.hpp>

namespace helios
{
    struct SemaphoreBuilder::SemaphoreBuilderImpl
    {
        IDevice* device = nullptr;
    };
} // namespace helios