#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanSurface;

    struct VulkanSwapchain final : ISwapchain
    {
        VulkanSwapchain() = default;
        ~VulkanSwapchain() override;
        [[nodiscard]] u32 imagesCount() const override;
        [[nodiscard]] vector<IImageView*> views() const override;
        [[nodiscard]] EFormat format() const override;
        [[nodiscard]] u32 acquireNextImage(const uint64_t wait,
                                           const ISemaphore* signal,
                                           const IFence* fence) override;

        [[nodiscard]] u32 width() const noexcept override;
        [[nodiscard]] u32 height() const noexcept override;

        bool destroyed = false;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanSurface* surface = nullptr;
        vector<IImageView*> imgViews;
        EFormat fmt = EFormat::UNDEFINED;
        u32 w = 0;
        u32 h = 0;
        HELIOS_NO_COPY_MOVE(VulkanSwapchain)
    };
} // namespace helios