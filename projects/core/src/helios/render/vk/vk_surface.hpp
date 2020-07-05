#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanContext;
    struct VulkanDevice;
    struct VulkanSwapchain;

    struct VulkanSurface final : ISurface
    {
        VulkanSurface() = default;
        ~VulkanSurface() override;
        [[nodiscard]] IWindow* window() const override;
        [[nodiscard]] SwapchainSupport swapchainSupport(const IPhysicalDevice* device) const override;

        VkSurfaceKHR surface = nullptr;

        VulkanContext* context = nullptr;
        VulkanDevice* device = nullptr;
        IWindow* windowPtr = nullptr;
        VulkanSwapchain* swapchain = nullptr;
        bool destroyed = false;

        HELIOS_NO_COPY_MOVE(VulkanSurface)
    };
} // namespace helios