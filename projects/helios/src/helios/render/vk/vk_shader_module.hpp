#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;

    struct VulkanShaderModule final : IShaderModule
    {
        VulkanShaderModule() = default;
        ~VulkanShaderModule() override;

        bool destroyed = false;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;

        HELIOS_NO_COPY_MOVE(VulkanShaderModule)
    };
} // namespace helios