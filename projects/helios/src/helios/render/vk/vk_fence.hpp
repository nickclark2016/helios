#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;

    struct VulkanFence final : IFence
    {
        VulkanFence() = default;
        ~VulkanFence() override;

        void reset() override;
        void wait() override;
        bool status() override;

        bool destroyed = false;
        VkFence fence = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;

        VulkanFence(const VulkanFence&) = delete;
        VulkanFence(VulkanFence&&) noexcept = delete;
        VulkanFence& operator=(const VulkanFence&) = delete;
        VulkanFence& operator=(VulkanFence&&) noexcept = delete;
    };
} // namespace helios