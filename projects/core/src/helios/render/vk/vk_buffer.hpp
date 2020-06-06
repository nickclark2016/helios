#pragma once

#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace helios
{
    struct VulkanDevice;

    struct VulkanBuffer final : IBuffer
    {
        VulkanBuffer() = default;
        ~VulkanBuffer();

        void* map() override;
        void unmap() override;

        HELIOS_NO_COPY_MOVE(VulkanBuffer)

        VulkanDevice* device;
        VkBuffer buf;
        VmaAllocation alloc;
        bool destroyed = false;
        EBufferTypeFlags type = 0;
    };
} // namespace helios