#pragma once

#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

namespace helios
{
    struct DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilderImpl
    {
        vector<DescriptorSetLayoutBinding> bindings;
        IDevice* device = nullptr;
    };
} // namespace helios