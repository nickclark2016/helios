#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    struct DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilderImpl
    {
        vector<DescriptorSetLayoutBinding> bindings;
        IDevice* device = nullptr;
    };
} // namespace helios