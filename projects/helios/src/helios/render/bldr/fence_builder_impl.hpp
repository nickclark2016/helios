#pragma once

#include <helios/render/graphics.hpp>

namespace helios
{
    struct FenceBuilder::FenceBuilderImpl
    {
        IDevice* device = nullptr;
        bool signalled = false;
    };
} // namespace helios