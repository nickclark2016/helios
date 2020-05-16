#include <helios/render/vk/vk_graphics_pipeline.hpp>

#include <helios/render/bldr/graphics_pipeline_buiilder_imp.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_pipeline_layout.hpp>
#include <helios/render/vk/vk_render_pass.hpp>
#include <helios/render/vk/vk_shader_module.hpp>

#include <glad/vulkan.h>

#include <algorithm>
#include <cstring>

namespace helios
{
    static const std::string shader_entry = "main";

    GraphicsPipelineBuilder::GraphicsPipelineBuilder()
    {
        _impl = new GraphicsPipelineBuilderImpl;
    }

    GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
    {
        delete _impl;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::allowDerivatives()
    {
        _impl->allowDerivatives = true;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::deriveFrom(
        const IGraphicsPipeline* parent)
    {
        _impl->parent = const_cast<IGraphicsPipeline*>(parent);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::vertex(
        const IShaderModule* vertex)
    {
        _impl->vertex = const_cast<IShaderModule*>(vertex);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::fragment(
        const IShaderModule* fragment)
    {
        _impl->fragment = const_cast<IShaderModule*>(fragment);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::input(
        const VertexInputState& bindings)
    {
        _impl->bindings = bindings;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::assembly(
        const InputAssemblyState& assembly)
    {
        _impl->input = assembly;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::tessellation(
        const TessellationState& tessellation)
    {
        _impl->tessellation = tessellation;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::viewports(
        const ViewportState& viewport)
    {
        _impl->viewports = viewport;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::rasterization(
        const RasterizationState& raster)
    {
        _impl->rasterization = raster;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::multisample(
        const MultisampleState& multi)
    {
        _impl->multisample = multi;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::depthStencil(
        const DepthStencilState& depth)
    {
        _impl->depth = depth;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::colorBlend(
        const ColorBlendState& color)
    {
        _impl->colors = color;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::dynamic(
        const DynamicState& dynamic)
    {
        _impl->dynamic = dynamic;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::layout(
        const IPipelineLayout* layout)
    {
        _impl->layout = const_cast<IPipelineLayout*>(layout);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::subpass(
        const uint32_t subpass)
    {
        _impl->subpass = subpass;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::renderpass(
        const IRenderPass* renderpass)
    {
        _impl->renderpass = const_cast<IRenderPass*>(renderpass);
        return *this;
    }

    IGraphicsPipeline* GraphicsPipelineBuilder::build() const
    {
        VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline;
        pipeline->layout = cast<VulkanPipelineLayout*>(_impl->layout);
        pipeline->device = pipeline->layout->device;
        pipeline->renderpass = cast<VulkanRenderPass*>(_impl->renderpass);

        vector<VkPipelineShaderStageCreateInfo> stages;
        {
            if (_impl->vertex)
            {
                VkPipelineShaderStageCreateInfo stage = {};
                stage.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
                stage.module =
                    cast<VulkanShaderModule*>(_impl->vertex)->shaderModule;
                stage.pName = shader_entry.data();

                stages.push_back(stage);
            }

            if (_impl->fragment)
            {
                VkPipelineShaderStageCreateInfo stage = {};
                stage.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                stage.module =
                    cast<VulkanShaderModule*>(_impl->fragment)->shaderModule;
                stage.pName = shader_entry.data();

                stages.push_back(stage);
            }
        }

        vector<VkVertexInputAttributeDescription> vertexAttribs;
        vector<VkVertexInputBindingDescription> vertexDescs;

        for (const auto& attribBinding : _impl->bindings.bindings)
        {
            VkVertexInputAttributeDescription attrib;
            VkVertexInputBindingDescription desc;

            attrib.binding = attribBinding.binding;
            attrib.format = static_cast<VkFormat>(attribBinding.format);
            attrib.location = attribBinding.location;
            attrib.offset = attribBinding.offset;

            desc.binding = attribBinding.binding;
            desc.inputRate =
                static_cast<VkVertexInputRate>(attribBinding.inputRate);
            desc.stride = attribBinding.stride;

            bool attribFound = false;
            for (const auto a : vertexAttribs)
            {
                if (a.location == attrib.location)
                {
                    attribFound = true;
                }
            }

            bool descFound = false;
            for (const auto d : vertexDescs)
            {
                if (d.binding == attrib.binding)
                {
                    descFound = true;
                }
            }

            if (!attribFound)
            {
                vertexAttribs.push_back(attrib);
            }

            if (!descFound)
            {
                vertexDescs.push_back(desc);
            }
        }

        vector<VkViewport> viewports;
        vector<VkRect2D> scissors;

        for (const auto& viewport : _impl->viewports.viewports)
        {
            viewports.push_back({viewport.x, viewport.y, viewport.width,
                                 viewport.height, viewport.minDepth,
                                 viewport.maxDepth});
        }

        for (const auto& scissor : _impl->viewports.scissorRegions)
        {
            scissors.push_back(
                {{scissor.x, scissor.y}, {scissor.width, scissor.height}});
        }

        vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        for (const auto& attachment : _impl->colors.attachments)
        {
            colorBlendAttachmentStates.push_back(
                {static_cast<VkBool32>(attachment.blendEnable ? VK_TRUE
                                                              : VK_FALSE),
                 static_cast<VkBlendFactor>(attachment.srcColorBlendFactor),
                 static_cast<VkBlendFactor>(attachment.dstColorBlendFactor),
                 static_cast<VkBlendOp>(attachment.colorBlendOp),
                 static_cast<VkBlendFactor>(attachment.srcAlphaBlendFactor),
                 static_cast<VkBlendFactor>(attachment.dstAlphaBlendFactor),
                 static_cast<VkBlendOp>(attachment.alphaBlendOp),
                 attachment.colorMask});
        }

        vector<VkDynamicState> dynamicStates;
        for (const auto& state : _impl->dynamic.states)
        {
            dynamicStates.push_back(static_cast<VkDynamicState>(state));
        }

        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexAttribs.size());
        vertexInput.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexDescs.size());
        vertexInput.pVertexAttributeDescriptions = vertexAttribs.data();
        vertexInput.pVertexBindingDescriptions = vertexDescs.data();

        VkPipelineInputAssemblyStateCreateInfo assembly = {};
        assembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assembly.primitiveRestartEnable =
            _impl->input.primitiveRestartEnable ? VK_TRUE : VK_FALSE;
        assembly.topology =
            static_cast<VkPrimitiveTopology>(_impl->input.topology);

        VkPipelineTessellationStateCreateInfo tessellation = {};
        tessellation.sType =
            VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellation.patchControlPoints =
            _impl->tessellation.patchControlPoints;

        VkPipelineViewportStateCreateInfo viewport = {};
        viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.scissorCount = static_cast<uint32_t>(scissors.size());
        viewport.viewportCount = static_cast<uint32_t>(viewports.size());
        viewport.pScissors = scissors.data();
        viewport.pViewports = viewports.data();

        VkPipelineRasterizationStateCreateInfo rasterization = {};
        rasterization.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.cullMode = _impl->rasterization.cullMode;
        rasterization.depthBiasClamp = _impl->rasterization.depthBiasClamp;
        rasterization.depthBiasConstantFactor =
            _impl->rasterization.depthBiasConstantFactor;
        rasterization.depthBiasEnable =
            _impl->rasterization.depthBiasEnable ? VK_TRUE : VK_FALSE;
        rasterization.depthBiasSlopeFactor =
            _impl->rasterization.depthBiasSlopeFactor;
        rasterization.depthClampEnable =
            _impl->rasterization.depthClampEnable ? VK_TRUE : VK_FALSE;
        rasterization.frontFace =
            static_cast<VkFrontFace>(_impl->rasterization.frontFace);
        rasterization.lineWidth = _impl->rasterization.lineWidth;
        rasterization.polygonMode =
            static_cast<VkPolygonMode>(_impl->rasterization.mode);
        rasterization.rasterizerDiscardEnable =
            _impl->rasterization.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE;

        uint32_t sampleMask = _impl->multisample.sampleMask.has_value()
                                  ? _impl->multisample.sampleMask.value()
                                  : 0;
        VkPipelineMultisampleStateCreateInfo multisample = {};
        multisample.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.alphaToCoverageEnable =
            _impl->multisample.alphaToCoverage ? VK_TRUE : VK_FALSE;
        multisample.alphaToOneEnable =
            _impl->multisample.alphaToOne ? VK_TRUE : VK_FALSE;
        multisample.minSampleShading = _impl->multisample.minSampleShading;
        multisample.pSampleMask =
            _impl->multisample.sampleMask.has_value() ? &sampleMask : nullptr;
        multisample.rasterizationSamples =
            static_cast<VkSampleCountFlagBits>(_impl->multisample.samples);
        multisample.sampleShadingEnable =
            _impl->multisample.sampleShadingEnable ? VK_TRUE : VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depth = {};
        depth.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth.back = {static_cast<VkStencilOp>(_impl->depth.back.failOp),
                      static_cast<VkStencilOp>(_impl->depth.back.passOp),
                      static_cast<VkStencilOp>(_impl->depth.back.depthFailOp),
                      static_cast<VkCompareOp>(_impl->depth.back.compareOp),
                      _impl->depth.back.compareMask,
                      _impl->depth.back.writeMask,
                      _impl->depth.back.reference};
        depth.depthBoundsTestEnable =
            _impl->depth.depthBoundsTestEnable ? VK_TRUE : VK_FALSE;
        depth.depthCompareOp =
            static_cast<VkCompareOp>(_impl->depth.depthCompareOp);
        depth.depthTestEnable =
            _impl->depth.depthTestEnable ? VK_TRUE : VK_FALSE;
        depth.depthWriteEnable =
            _impl->depth.depthWriteEnable ? VK_TRUE : VK_FALSE;
        depth.front = {static_cast<VkStencilOp>(_impl->depth.front.failOp),
                       static_cast<VkStencilOp>(_impl->depth.front.passOp),
                       static_cast<VkStencilOp>(_impl->depth.front.depthFailOp),
                       static_cast<VkCompareOp>(_impl->depth.front.compareOp),
                       _impl->depth.front.compareMask,
                       _impl->depth.front.writeMask,
                       _impl->depth.front.reference};
        depth.maxDepthBounds = _impl->depth.maxDepthBounds;
        depth.minDepthBounds = _impl->depth.minDepthBounds;
        depth.stencilTestEnable = _impl->depth.stencilTestEnable;

        VkPipelineColorBlendStateCreateInfo colorBlend = {};
        colorBlend.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.attachmentCount =
            static_cast<uint32_t>(colorBlendAttachmentStates.size());
        memcpy(colorBlend.blendConstants, _impl->colors.blendConstants,
               4 * sizeof(float));
        colorBlend.logicOp = static_cast<VkLogicOp>(_impl->colors.logicOp);
        colorBlend.logicOpEnable =
            _impl->colors.logicEnable ? VK_TRUE : VK_FALSE;
        colorBlend.pAttachments = colorBlendAttachmentStates.data();

        VkPipelineDynamicStateCreateInfo dynamic = {};
        dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic.dynamicStateCount =
            static_cast<uint32_t>(_impl->dynamic.states.size());
        dynamic.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.flags |= _impl->allowDerivatives
                          ? VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT
                          : 0;
        info.flags |= _impl->parent ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0;
        info.stageCount = static_cast<uint32_t>(stages.size());
        info.pStages = stages.data();
        info.pVertexInputState = &vertexInput;
        info.pInputAssemblyState = &assembly;
        info.pTessellationState = &tessellation;
        info.pViewportState = &viewport;
        info.pRasterizationState = &rasterization;
        info.pMultisampleState = &multisample;
        info.pDepthStencilState = &depth;
        info.pColorBlendState = &colorBlend;
        info.pDynamicState = &dynamic;
        info.layout = pipeline->layout->layout;
        info.subpass = _impl->subpass;
        info.basePipelineHandle =
            _impl->parent
                ? cast<VulkanGraphicsPipeline*>(_impl->parent)->pipeline
                : nullptr;
        info.basePipelineIndex = 0;
        info.renderPass = pipeline->renderpass->renderpass;

        vkCreateGraphicsPipelines(pipeline->device->device, nullptr, 1, &info,
                                  nullptr, &pipeline->pipeline);
        pipeline->device->graphicsPipelines.push_back(pipeline);

        return pipeline;
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        if (!destroyed)
        {
            if (!device->destroyed)
            {
                device->graphicsPipelines.erase(
                    std::find(device->graphicsPipelines.begin(),
                              device->graphicsPipelines.end(), this));
            }

            vkDestroyPipeline(device->device, pipeline, nullptr);
        }
    }
} // namespace helios