#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanDescriptorSet;
    struct VulkanDevice;

    struct VulkanDescriptorPool final : IDescriptorPool
    {
        VulkanDescriptorPool() = default;
        ~VulkanDescriptorPool();

        vector<IDescriptorSet*> allocate(
            const vector<IDescriptorSetLayout*>& layouts) override;
        void reset() override;

        u32 maxSets() const override;
        u32 allocatedSets() const override;
        u32 maxSamplers() const override;
        u32 allocatedSamplers() const override;
        u32 maxCombinedImageSamplers() const override;
        u32 allocatedCombinedImageSamplers() const override;
        u32 maxSampledImages() const override;
        u32 allocatedSampledImages() const override;
        u32 maxStorageImages() const override;
        u32 allocatedStorageImages() const override;
        u32 maxUniformTexelBuffers() const override;
        u32 allocatedUniformTexelBuffers() const override;
        u32 maxStorageTexelBuffers() const override;
        u32 allocatedStorageTexelBuffers() const override;
        u32 maxUniformBuffers() const override;
        u32 allocatedUniformBuffers() const override;
        u32 maxStorageBuffers() const override;
        u32 allocatedStorageBuffers() const override;
        u32 maxDynamicUniformBuffers() const override;
        u32 allocatedDynamicUniformBuffers() const override;
        u32 maxDynamicStorageBuffers() const override;
        u32 allocatedDynamicStorageBuffers() const override;
        u32 maxInputAttachments() const override;
        u32 allocatedInputAttachments() const override;

        HELIOS_NO_COPY_MOVE(VulkanDescriptorPool)

        VkDescriptorPool pool;
        VulkanDevice* device;
        vector<VulkanDescriptorSet*> sets;
        vector<VkDescriptorSet> vkSets;

        bool destroyed = false;

        u32 maxSetCount;
        u32 allocatedSetCount;
        u32 maxSamplerCount;
        u32 allocatedSamplerCount;
        u32 maxImageSamplerCount;
        u32 allocatedImageSamplerCount;
        u32 maxSampledImageCount;
        u32 allocatedSampledImageCount;
        u32 maxStorageImageCount;
        u32 allocatedStorageImageCount;
        u32 maxUniformTexelBufferCount;
        u32 allocatedUniformTexelBufferCount;
        u32 maxStorageTexelBufferCount;
        u32 allocatedStorageTexelBufferCount;
        u32 maxUniformBufferCount;
        u32 allocatedUniformBufferCount;
        u32 maxStorageBufferCount;
        u32 allocatedStorageBufferCount;
        u32 maxUniformDynamicCount;
        u32 allocatedUniformDynamicCount;
        u32 maxStorageDynamicCount;
        u32 allocatedStorageDynamicCount;
        u32 maxInputAttachmentCount;
        u32 allocatedInputAttachmentCount;
    };
} // namespace helios