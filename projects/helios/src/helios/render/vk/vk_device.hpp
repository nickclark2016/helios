#pragma once

#include <helios/containers/memory.hpp>
#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace helios
{
    struct VulkanBuffer;
    struct VulkanCommandPool;
    struct VulkanDescriptorSetLayout;
    struct VulkanFence;
    struct VulkanGraphicsPipeline;
    struct VulkanImage;
    struct VulkanPhysicalDevice;
    struct VulkanPipelineLayout;
    struct VulkanRenderPass;
    struct VulkanSemaphore;
    struct VulkanShaderModule;
    struct VulkanSurface;

    struct VulkanDevice final : IDevice
    {
        VulkanDevice() = default;
        ~VulkanDevice();

        [[nodiscard]] vector<IQueue*> queues() const override;
        void idle() const override;

        bool destroyed = false;
        VkDevice device = VK_NULL_HANDLE;
        vector<IQueue*> deviceQueues;
        vector<VulkanBuffer*> buffers;
        vector<VulkanCommandPool*> commandBufferPools;
        vector<VulkanDescriptorSetLayout*> setLayouts;
        vector<VulkanFence*> fences;
        vector<VulkanGraphicsPipeline*> graphicsPipelines;
        vector<VulkanImage*> images;
        vector<VulkanPipelineLayout*> pipelineLayouts;
        vector<VulkanRenderPass*> renderPasses;
        vector<VulkanSemaphore*> sems;
        vector<VulkanShaderModule*> modules;
        vector<VulkanSurface*> surfaces;
        VulkanPhysicalDevice* parent = nullptr;
        VmaAllocator memAllocator;
        dynamic_block_allocator<u8, 64 * 1024, 256, EMemoryTag::VERTEX_BUFFER>
            vbAllocator;
        dynamic_block_allocator<u8, 64 * 1024, 256, EMemoryTag::INDEX_BUFFER>
            ibAllocator;
        dynamic_block_allocator<u8, 64 * 1024, 256, EMemoryTag::UNIFORM_BUFFER>
            ubAllocator;
        dynamic_block_allocator<u8, 64 * 1024, 256,
                                EMemoryTag::SHADER_STORAGE_BUFFER>
            ssbAllocator;
        dynamic_block_allocator<u8, 256 * 1024, 256, EMemoryTag::TEXTURE_BUFFER>
            texAllocator;

        HELIOS_NO_COPY_MOVE(VulkanDevice)
    };
} // namespace helios