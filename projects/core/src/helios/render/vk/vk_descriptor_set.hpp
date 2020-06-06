#pragma once

#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDescriptorPool;

    struct VulkanDescriptorSet final : IDescriptorSet
    {
        VulkanDescriptorSet() = default;
        ~VulkanDescriptorSet() override;

        void write(const vector<DescriptorWriteInfo>& descriptors) override;

        HELIOS_NO_COPY_MOVE(VulkanDescriptorSet)

        VulkanDescriptorPool* pool;
        VkDescriptorSet set;
        bool destroyed = false;
    };
} // namespace helios