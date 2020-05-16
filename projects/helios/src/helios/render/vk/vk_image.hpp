#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanImageView;

    struct VulkanImage final : IImage
    {
        VulkanImage() = default;
        ~VulkanImage() override;

        bool destroyed = false;
        VkImage image = VK_NULL_HANDLE;
        bool owned = false;
        VulkanDevice* device = nullptr;

        vector<VulkanImageView*> views;

        HELIOS_NO_COPY_MOVE(VulkanImage)
    };
} // namespace helios