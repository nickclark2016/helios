#pragma once

#include <helios/render/graphics.hpp>

namespace helios
{
    struct GraphicsPipelineBuilder::GraphicsPipelineBuilderImpl
    {
        IGraphicsPipeline* parent = nullptr;
        IShaderModule* vertex = nullptr;
        IShaderModule* fragment = nullptr;
        VertexInputState bindings;
        InputAssemblyState input = {};
        TessellationState tessellation = {1};
        ViewportState viewports;
        RasterizationState rasterization;
        MultisampleState multisample;
        DepthStencilState depth;
        ColorBlendState colors;
        DynamicState dynamic;
        IPipelineLayout* layout = nullptr;
        u32 subpass = 0;
        IRenderPass* renderpass = nullptr;
        bool allowDerivatives = false;
    };
} // namespace helios