#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanFramebuffer;

    struct VulkanRenderPass final : IRenderPass
    {
        VulkanRenderPass() = default;
        ~VulkanRenderPass() override;

        bool destroyed = false;
        VkRenderPass renderpass = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        vector<VulkanFramebuffer*> framebuffers;

        VulkanRenderPass(const VulkanRenderPass&) = delete;
        VulkanRenderPass(VulkanRenderPass&&) noexcept = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
        VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = delete;
    };
} // namespace helios