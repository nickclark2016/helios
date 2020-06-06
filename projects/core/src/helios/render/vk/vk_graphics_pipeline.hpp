#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanPipelineLayout;
    struct VulkanRenderPass;

    struct VulkanGraphicsPipeline final : IGraphicsPipeline
    {
        VulkanGraphicsPipeline() = default;
        ~VulkanGraphicsPipeline() override;

        bool destroyed = false;
        VkPipeline pipeline = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanPipelineLayout* layout = nullptr;
        VulkanRenderPass* renderpass = nullptr;

        HELIOS_NO_COPY_MOVE(VulkanGraphicsPipeline)
    };
} // namespace helios