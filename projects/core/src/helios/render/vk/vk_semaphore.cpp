#include <helios/render/vk/vk_semaphore.hpp>

#include <helios/render/bldr/semaphore_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    SemaphoreBuilder::SemaphoreBuilder()
    {
        _impl = new SemaphoreBuilderImpl;
    }

    SemaphoreBuilder::~SemaphoreBuilder()
    {
        delete _impl;
    }

    SemaphoreBuilder& SemaphoreBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    ISemaphore* SemaphoreBuilder::build() const
    {
        VulkanSemaphore* sem = new VulkanSemaphore;
        sem->device = cast<VulkanDevice*>(_impl->device);

        VkSemaphoreTypeCreateInfo type = {};
        type.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        type.initialValue = 0;
        type.semaphoreType = VK_SEMAPHORE_TYPE_BINARY;

        VkSemaphoreCreateInfo info = {};
        info.pNext = &type;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(sem->device->device, &info, nullptr, &sem->semaphore);
        sem->device->sems.push_back(sem);

        return sem;
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->sems.erase(
                    std::find(device->sems.begin(), device->sems.end(), this));
            }

            vkDestroySemaphore(device->device, semaphore, nullptr);
        }
    }
} // namespace helios