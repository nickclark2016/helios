#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

#include <cstdint>

namespace helios
{
    struct VulkanQueue final : IQueue
    {
        VulkanQueue() = default;
        ~VulkanQueue() override = default;

        [[nodiscard]] f32 priority() const override;
        [[nodiscard]] u32 index() override;
        [[nodiscard]] IPhysicalDevice::QueueProperties props() const override;
        [[nodiscard]] bool canPresent(const IPhysicalDevice* device,
                                      const ISurface* surface) const override;
        void submit(const vector<SubmitInfo>& submitInfo,
                    const IFence* fence) const override;
        void present(const PresentInfo& presentInfo) const override;

        bool destroyed = false;
        IPhysicalDevice::QueueProperties family = {};
        u32 queueIndex = 0;
        f32 queuePriority = 0.0f;
        VkQueue queue = VK_NULL_HANDLE;

        HELIOS_NO_COPY_MOVE(VulkanQueue)
    };
} // namespace helios