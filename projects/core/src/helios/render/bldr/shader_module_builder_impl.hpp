#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    struct ShaderModuleBuilder::ShaderModuleBuilderImpl
    {
        vector<uint8_t> data;
        IDevice* device = nullptr;
    };
} // namespace helios