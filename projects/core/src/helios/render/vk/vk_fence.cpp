#include <helios/render/vk/vk_fence.hpp>

#include <helios/render/bldr/fence_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    FenceBuilder::FenceBuilder()
    {
        _impl = new FenceBuilderImpl;
    }

    FenceBuilder::~FenceBuilder()
    {
        delete _impl;
    }

    FenceBuilder& FenceBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    FenceBuilder& FenceBuilder::signaled()
    {
        _impl->signalled = true;
        return *this;
    }

    IFence* FenceBuilder::build() const
    {
        VulkanFence* fence = new VulkanFence;

        VkFenceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.flags |= _impl->signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        fence->device = cast<VulkanDevice*>(_impl->device);
        vkCreateFence(fence->device->device, &info, nullptr, &fence->fence);
        fence->device->fences.push_back(fence);

        return fence;
    }

    VulkanFence::~VulkanFence()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->fences.erase(std::find(device->fences.begin(), device->fences.end(), this));
            }

            vkDestroyFence(device->device, fence, nullptr);
        }
    }

    void VulkanFence::reset()
    {
        vkResetFences(device->device, 1, &fence);
    }

    void VulkanFence::wait()
    {
        vkWaitForFences(device->device, 1, &fence, VK_TRUE, UINT64_MAX);
    }

    bool VulkanFence::status()
    {
        return vkGetFenceStatus(device->device, fence) == VK_SUCCESS;
    }
} // namespace helios