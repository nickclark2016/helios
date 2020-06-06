#pragma once

#include <helios/render/graphics.hpp>

namespace helios
{
    struct CommandPoolBuilder::CommandPoolBuilderImpl
    {
        IDevice* device = nullptr;
        IQueue* queue = nullptr;
        bool transient = false;
        bool reset = false;
    };
} // namespace helios