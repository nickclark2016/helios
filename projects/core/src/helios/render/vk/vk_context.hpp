#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

#include <functional>

namespace helios
{
    struct VulkanPhysicalDevice;

    struct VulkanContext final : IContext
    {
        VulkanContext() = default;
        ~VulkanContext();

        [[nodiscard]] vector<IPhysicalDevice*> physicalDevices() const override;

        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        std::function<void(EMessageSeverity, const char*)> debugCallback;
        bool destroyed = false;
        vector<VulkanPhysicalDevice*> devices;

        HELIOS_NO_COPY_MOVE(VulkanContext)
    };
} // namespace helios