#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDevice;

    struct VulkanDescriptorSetLayout final : IDescriptorSetLayout
    {
        VulkanDescriptorSetLayout() = default;
        ~VulkanDescriptorSetLayout() override;
        [[nodiscard]] vector<DescriptorSetLayoutBinding> bindings() const override;

        bool destroyed = false;
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        vector<DescriptorSetLayoutBinding> layoutBindings;

        HELIOS_NO_COPY_MOVE(VulkanDescriptorSetLayout)
    };
} // namespace helios