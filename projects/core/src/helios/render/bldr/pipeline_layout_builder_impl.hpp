#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    struct PipelineLayoutBuilder::PipelineLayoutBuilderImpl
    {
        IDevice* device = nullptr;
        vector<PushConstantRange> ranges;
        vector<IDescriptorSetLayout*> layouts;
    };
} // namespace helios