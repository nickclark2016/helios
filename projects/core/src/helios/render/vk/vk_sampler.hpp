#pragma once

#include <helios/render/graphics.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanSampler final : ISampler
    {
        VulkanSampler() = default;
        ~VulkanSampler() override;

        bool destroyed = false;
        VulkanDevice* device;
        VkSampler sampler;

        HELIOS_NO_COPY_MOVE(VulkanSampler)
    };
} // namespace helios
