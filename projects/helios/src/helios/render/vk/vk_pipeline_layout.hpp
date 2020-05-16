#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanDescriptorSetLayout;

    struct VulkanPipelineLayout final : IPipelineLayout
    {
        VulkanPipelineLayout() = default;
        ~VulkanPipelineLayout() override;

        bool destroyed = false;
        VkPipelineLayout layout = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        vector<PushConstantRange> ranges;
        vector<VulkanDescriptorSetLayout*> layouts;

        HELIOS_NO_COPY_MOVE(VulkanPipelineLayout)
    };
} // namespace helios