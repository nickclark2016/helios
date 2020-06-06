#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanContext;
    struct VulkanDevice;

    struct VulkanPhysicalDevice final : IPhysicalDevice
    {
        VulkanPhysicalDevice() = default;
        ~VulkanPhysicalDevice();

        [[nodiscard]] std::string name() const override;
        [[nodiscard]] EPhysicalDeviceType type() const override;
        [[nodiscard]] Features features() const override;
        [[nodiscard]] vector<QueueProperties> queueProperties() override;

        bool destroyed = false;
        VkPhysicalDevice device = VK_NULL_HANDLE;
        std::string deviceName = "";
        EPhysicalDeviceType deviceType = EPhysicalDeviceType::OTHER;
        VulkanContext* context = nullptr;
        VulkanDevice* logicalDevice = VK_NULL_HANDLE;

        HELIOS_NO_COPY_MOVE(VulkanPhysicalDevice)
    };
} // namespace helios