#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanImage;

    struct VulkanImageView final : IImageView
    {
        VulkanImageView() = default;
        ~VulkanImageView() override;

        IImage* image() override;

        bool destroyed = false;
        VkImageView view = VK_NULL_HANDLE;
        VulkanImage* img = nullptr;

        HELIOS_NO_COPY_MOVE(VulkanImageView)
    };
} // namespace helios