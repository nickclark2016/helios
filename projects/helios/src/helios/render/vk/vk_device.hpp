#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanCommandPool;
    struct VulkanDescriptorSetLayout;
    struct VulkanFence;
    struct VulkanGraphicsPipeline;
    struct VulkanImage;
    struct VulkanPhysicalDevice;
    struct VulkanPipelineLayout;
    struct VulkanRenderPass;
    struct VulkanSemaphore;
    struct VulkanShaderModule;
    struct VulkanSurface;

    struct VulkanDevice final : IDevice
    {
        VulkanDevice() = default;
        ~VulkanDevice();

        [[nodiscard]] vector<IQueue*> queues() const override;
        void idle() const override;

        bool destroyed = false;
        VkDevice device = VK_NULL_HANDLE;
        vector<IQueue*> deviceQueues;
        vector<VulkanCommandPool*> commandBufferPools;
        vector<VulkanDescriptorSetLayout*> setLayouts;
        vector<VulkanFence*> fences;
        vector<VulkanGraphicsPipeline*> graphicsPipelines;
        vector<VulkanImage*> images;
        vector<VulkanPipelineLayout*> pipelineLayouts;
        vector<VulkanRenderPass*> renderPasses;
        vector<VulkanSemaphore*> sems;
        vector<VulkanShaderModule*> modules;
        vector<VulkanSurface*> surfaces;
        VulkanPhysicalDevice* parent = nullptr;

        HELIOS_NO_COPY_MOVE(VulkanDevice)
    };
} // namespace helios