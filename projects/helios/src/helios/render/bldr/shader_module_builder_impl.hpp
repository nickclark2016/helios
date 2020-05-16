#pragma once

#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

namespace helios
{
    struct ShaderModuleBuilder::ShaderModuleBuilderImpl
    {
        vector<uint8_t> data;
        IDevice* device = nullptr;
    };
} // namespace helios