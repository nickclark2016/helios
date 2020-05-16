#pragma once

#include <helios/core/window.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    struct SurfaceBuilder::SurfaceBuilderImpl
    {
        IWindow* window = nullptr;
        IDevice* device = nullptr;
    };
} // namespace helios