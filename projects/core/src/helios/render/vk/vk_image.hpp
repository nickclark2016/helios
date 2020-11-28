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

        EFormat format() const noexcept override;
        u32 width() const noexcept override;
        u32 height() const noexcept override;
        u32 layers() const noexcept override;

        bool destroyed = false;
        VkImage image = VK_NULL_HANDLE;
        bool owned = false;
        VulkanDevice* device = nullptr;
        VmaAllocation allocation;
        EFormat fmt;
        u32 w;
        u32 h;
        u32 l;

        vector<VulkanImageView*> views;

        HELIOS_NO_COPY_MOVE(VulkanImage)
    };
} // namespace helios