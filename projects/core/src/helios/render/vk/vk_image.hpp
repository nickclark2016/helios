#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace helios
{
    struct VulkanDevice;
    struct VulkanImageView;

    struct VulkanImage final : IImage
    {
        VulkanImage() = default;
        ~VulkanImage() override;

        EFormat getFormat() const noexcept override;

        bool destroyed = false;
        VkImage image = VK_NULL_HANDLE;
        bool owned = false;
        VulkanDevice* device = nullptr;
        VmaAllocation allocation;
        EFormat format;

        vector<VulkanImageView*> views;

        HELIOS_NO_COPY_MOVE(VulkanImage)
    };
} // namespace helios