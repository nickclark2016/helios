#pragma once

#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

namespace helios
{
    struct PipelineLayoutBuilder::PipelineLayoutBuilderImpl
    {
        IDevice* device = nullptr;
        vector<PushConstantRange> ranges;
        vector<IDescriptorSetLayout*> layouts;
    };
} // namespace helios