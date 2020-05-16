#pragma once

#include <helios/macros.hpp>
#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanSurface;

    struct VulkanSwapchain final : ISwapchain
    {
        VulkanSwapchain() = default;
        ~VulkanSwapchain() override;
        [[nodiscard]] uint32_t imagesCount() const override;
        [[nodiscard]] vector<IImageView*> views() const override;
        [[nodiscard]] EFormat format() const override;
        [[nodiscard]] uint32_t acquireNextImage(const uint64_t wait,
                                                const ISemaphore* signal,
                                                const IFence* fence) override;

        bool destroyed = false;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanSurface* surface = nullptr;
        vector<IImageView*> imgViews;
        EFormat fmt = EFormat::UNDEFINED;

        HELIOS_NO_COPY_MOVE(VulkanSwapchain)
    };
} // namespace helios