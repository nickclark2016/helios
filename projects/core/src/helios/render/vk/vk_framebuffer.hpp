#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

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

        [[nodiscard]] u32 width() const override;
        [[nodiscard]] u32 height() const override;
        [[nodiscard]] u32 layers() const override;
        [[nodiscard]] vector<IImageView*> attachments() const override;

        bool destroyed = false;
        VulkanRenderPass* pass = nullptr;
        vector<VulkanImageView*> images;
        VkFramebuffer fb = VK_NULL_HANDLE;
        u32 fbWidth = 0;
        u32 fbHeight = 0;
        u32 fbLayers = 0;

        HELIOS_NO_COPY_MOVE(VulkanFramebuffer)
    };
} // namespace helios