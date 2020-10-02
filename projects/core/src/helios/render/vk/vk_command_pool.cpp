#include <helios/render/vk/vk_command_pool.hpp>

#include <helios/render/bldr/command_pool_builder_impl.hpp>
#include <helios/render/vk/vk_command_buffer.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_queue.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    CommandPoolBuilder::CommandPoolBuilder()
    {
        _impl = new CommandPoolBuilderImpl;
    }

    CommandPoolBuilder::~CommandPoolBuilder()
    {
        delete _impl;
    }

    CommandPoolBuilder& CommandPoolBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    CommandPoolBuilder& CommandPoolBuilder::queue(const IQueue* queue)
    {
        _impl->queue = const_cast<IQueue*>(queue);
        return *this;
    }

    CommandPoolBuilder& CommandPoolBuilder::transient()
    {
        _impl->transient = true;
        return *this;
    }

    CommandPoolBuilder& CommandPoolBuilder::reset()
    {
        _impl->reset = true;
        return *this;
    }

    ICommandPool* CommandPoolBuilder::build() const
    {
        VulkanCommandPool* pool = new VulkanCommandPool;
        pool->queue = cast<VulkanQueue*>(_impl->queue);
        pool->device = cast<VulkanDevice*>(_impl->device);

        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags |=
            _impl->transient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
        info.flags |=
            _impl->reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
        info.queueFamilyIndex = pool->queue->family.index;

        vkCreateCommandPool(pool->device->device, &info, nullptr, &pool->pool);
        pool->device->commandBufferPools.push_back(pool);

        return pool;
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (device->destroyed == false)
            {
                auto it = std::find(device->commandBufferPools.begin(), device->commandBufferPools.end(), this);
                if (it != device->commandBufferPools.end())
                {
                    device->commandBufferPools.erase(it);
                }
            }

            for (const auto& buf : buffers)
            {
                delete buf;
            }

            vkDestroyCommandPool(device->device, pool, nullptr);
        }
    }

    ICommandBuffer* VulkanCommandPool::allocate(const ECommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandBufferCount = 1;
        info.commandPool = pool;
        info.level = level == ECommandBufferLevel::PRIMARY
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        VkCommandBuffer buf;
        vkAllocateCommandBuffers(device->device, &info, &buf);

        VulkanCommandBuffer* buffer = new VulkanCommandBuffer;
        buffer->buffer = buf;
        buffer->pool = this;
        buffers.push_back(buffer);

        return buffer;
    }

    vector<ICommandBuffer*> VulkanCommandPool::allocate(
        const u32 count, const ECommandBufferLevel level)
    {
        vector<VkCommandBuffer> bufs(count);

        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandBufferCount = count;
        info.commandPool = pool;
        info.level = level == ECommandBufferLevel::PRIMARY
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        vkAllocateCommandBuffers(device->device, &info, bufs.data());

        vector<ICommandBuffer*> buffers;
        buffers.reserve(count);

        for (const auto& buf : bufs)
        {
            const auto buffer = new VulkanCommandBuffer;
            buffer->buffer = buf;
            buffer->pool = this;
            buffers.push_back(buffer);
            this->buffers.push_back(buffer);
        }

        return buffers;
    }
} // namespace helios