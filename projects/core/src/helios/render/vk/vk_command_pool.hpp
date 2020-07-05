#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanCommandBuffer;
    struct VulkanDevice;
    struct VulkanQueue;

    struct VulkanCommandPool final : ICommandPool
    {
        VulkanCommandPool() = default;
        ~VulkanCommandPool() override;

        [[nodiscard]] ICommandBuffer* allocate(const ECommandBufferLevel level) override;
        [[nodiscard]] vector<ICommandBuffer*> allocate(const u32 count, const ECommandBufferLevel level) override;

        bool destroyed = false;
        VkCommandPool pool = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanQueue* queue = nullptr;

        vector<VulkanCommandBuffer*> buffers;

        HELIOS_NO_COPY_MOVE(VulkanCommandPool)
    };
} // namespace helios