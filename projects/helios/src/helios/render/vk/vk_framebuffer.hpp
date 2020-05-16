#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <glad/vulkan.h>

#include <cstdint>

namespace helios
{
    struct VulkanImageView;
    struct VulkanRenderPass;

    struct VulkanFramebuffer final : IFramebuffer
    {
        VulkanFramebuffer() = default;
        ~VulkanFramebuffer() override;

        [[nodiscard]] uint32_t width() const override;
        [[nodiscard]] uint32_t height() const override;
        [[nodiscard]] uint32_t layers() const override;
        [[nodiscard]] vector<IImageView*> attachments() const override;

        bool destroyed = false;
        VulkanRenderPass* pass = nullptr;
        vector<VulkanImageView*> images;
        VkFramebuffer fb = VK_NULL_HANDLE;
        uint32_t fbWidth = 0;
        uint32_t fbHeight = 0;
        uint32_t fbLayers = 0;

        HELIOS_NO_COPY_MOVE(VulkanFramebuffer)
    };
} // namespace helios