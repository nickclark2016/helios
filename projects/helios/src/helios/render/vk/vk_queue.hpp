#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <glad/vulkan.h>

#include <cstdint>

namespace helios
{
    struct VulkanQueue final : IQueue
    {
        VulkanQueue() = default;
        ~VulkanQueue() override = default;

        [[nodiscard]] float priority() const override;
        [[nodiscard]] uint32_t index() override;
        [[nodiscard]] IPhysicalDevice::QueueProperties props() const override;
        [[nodiscard]] bool canPresent(const IPhysicalDevice* device,
                                      const ISurface* surface) const override;
        void submit(const vector<SubmitInfo>& submitInfo,
                    const IFence* fence) const override;
        void present(const PresentInfo& presentInfo) const override;

        bool destroyed = false;
        IPhysicalDevice::QueueProperties family = {};
        uint32_t queueIndex = 0;
        float queuePriority = 0.0f;
        VkQueue queue = VK_NULL_HANDLE;

        HELIOS_NO_COPY_MOVE(VulkanQueue)
    };
} // namespace helios