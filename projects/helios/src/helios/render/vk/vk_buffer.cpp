#include <helios/render/vk/vk_buffer.hpp>

#include <helios/containers/vector.hpp>
#include <helios/render/bldr/buffer_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_queue.hpp>

#include <glad/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace helios
{
    BufferBuilder::BufferBuilder()
    {
        _impl = new BufferBuilderImpl;
    }

    BufferBuilder::~BufferBuilder()
    {
        delete _impl;
    }

    BufferBuilder& BufferBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    BufferBuilder& BufferBuilder::size(u64 bytes)
    {
        _impl->size = bytes;
        return *this;
    }

    BufferBuilder& BufferBuilder::queues(
        const vector<IQueue*>& concurrentAccessQueues)
    {
        _impl->queues = concurrentAccessQueues;
        return *this;
    }

    BufferBuilder& BufferBuilder::usage(const EBufferTypeFlags usage)
    {
        _impl->usage = usage;
        return *this;
    }

    BufferBuilder& BufferBuilder::preferredFlags(
        const EMemoryPropertyFlags flags)
    {
        _impl->preferred = flags;
        return *this;
    }

    BufferBuilder& BufferBuilder::requiredFlags(
        const EMemoryPropertyFlags flags)
    {
        _impl->required = flags;
        return *this;
    }

    BufferBuilder& BufferBuilder::memoryUsage(const EMemoryUsage usage)
    {
        _impl->memoryUsage = usage;
        return *this;
    }

    IBuffer* BufferBuilder::build() const
    {
        VulkanBuffer* buffer = new VulkanBuffer;
        buffer->device = cast<VulkanDevice*>(_impl->device);

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

        vector<u32> queues;

        for (IQueue* queue : _impl->queues)
        {
            const u32 idx = cast<VulkanQueue*>(queue)->index();
            if (std::find(queues.begin(), queues.end(), idx) == queues.end())
            {
                queues.push_back(idx);
            }
        }

        bufferInfo.queueFamilyIndexCount = static_cast<u32>(queues.size());
        if (bufferInfo.queueFamilyIndexCount != 0)
        {
            bufferInfo.pQueueFamilyIndices = queues.data();
        }

        bufferInfo.size = _impl->size;
        bufferInfo.sharingMode = queues.empty() ? VK_SHARING_MODE_EXCLUSIVE
                                                : VK_SHARING_MODE_CONCURRENT;
        bufferInfo.usage = _impl->usage;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.requiredFlags = _impl->required;
        allocInfo.preferredFlags = _impl->preferred;

        switch (_impl->memoryUsage)
        {
        case EMemoryUsage::CPU_ONLY:
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            break;
        case EMemoryUsage::GPU_ONLY:
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            break;
        case EMemoryUsage::CPU_TO_GPU:
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            break;
        case EMemoryUsage::GPU_TO_CPU:
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
            break;
        }

        vmaCreateBuffer(buffer->device->memAllocator, &bufferInfo, &allocInfo,
                        &buffer->buf, &buffer->alloc, nullptr);

        buffer->type = _impl->usage;
        buffer->device->buffers.push_back(buffer);

        return buffer;
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (!destroyed)
        {
            destroyed = true;
            device->buffers.erase(std::find(device->buffers.begin(),
                                            device->buffers.end(), this));

            vmaDestroyBuffer(device->memAllocator, buf, alloc);
        }
    }

    void* VulkanBuffer::map()
    {
        void* buf;
        vmaMapMemory(device->memAllocator, alloc, &buf);
        return buf;
    }

    void VulkanBuffer::unmap()
    {
        vmaUnmapMemory(device->memAllocator, alloc);
    }
} // namespace helios