#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;

    struct VulkanSemaphore final : ISemaphore
    {
        VulkanSemaphore() = default;
        ~VulkanSemaphore() override;

        bool destroyed = false;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;

        HELIOS_NO_COPY_MOVE(VulkanSemaphore)
    };
} // namespace helios