#include <helios/render/vk/vk_queue.hpp>

#include <helios/containers/utility.hpp>
#include <helios/render/vk/vk_command_buffer.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_fence.hpp>
#include <helios/render/vk/vk_physical_device.hpp>
#include <helios/render/vk/vk_semaphore.hpp>
#include <helios/render/vk/vk_surface.hpp>
#include <helios/render/vk/vk_swapchain.hpp>

#include <glad/vulkan.h>

namespace helios
{
    f32 VulkanQueue::priority() const
    {
        return queuePriority;
    }

    u32 VulkanQueue::index()
    {
        return queueIndex;
    }

    IPhysicalDevice::QueueProperties VulkanQueue::props() const
    {
        return family;
    }

    bool VulkanQueue::canPresent(const IPhysicalDevice* device,
                                 const ISurface* surface) const
    {
        VkBool32 present;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            cast<const VulkanPhysicalDevice*>(device)->device, family.index,
            cast<const VulkanSurface*>(surface)->surface, &present);
        return present == VK_TRUE;
    }

    void VulkanQueue::submit(const vector<SubmitInfo>& submitInfo,
                             const IFence* fence) const
    {
        vector<VkSubmitInfo> infos;
        infos.reserve(submitInfo.size());

        vector<vector<VkCommandBuffer>> buffers;
        vector<vector<VkSemaphore>> wait;
        vector<vector<VkSemaphore>> signal;
        vector<vector<VkPipelineStageFlags>> stages;

        buffers.reserve(submitInfo.size());
        wait.reserve(submitInfo.size());
        signal.reserve(submitInfo.size());
        stages.reserve(submitInfo.size());

        for (const auto& si : submitInfo)
        {
            vector<VkCommandBuffer> bufs;
            bufs.reserve(si.buffers.size());

            vector<VkSemaphore> waits;
            waits.reserve(si.wait.size());

            vector<VkSemaphore> signals;
            signals.reserve(si.signal.size());

            vector<VkPipelineStageFlags> flags;
            flags.reserve(si.waitMask.size());

            for (const auto& buf : si.buffers)
            {
                bufs.push_back(cast<VulkanCommandBuffer*>(buf)->buffer);
            }

            for (const auto& sem : si.wait)
            {
                waits.push_back(cast<VulkanSemaphore*>(sem)->semaphore);
            }

            for (const auto& sem : si.signal)
            {
                signals.push_back(cast<VulkanSemaphore*>(sem)->semaphore);
            }

            for (const EPipelineStageFlags flag : si.waitMask)
            {
                flags.push_back(static_cast<VkPipelineStageFlags>(flag));
            }

            buffers.push_back(bufs);
            wait.push_back(waits);
            signal.push_back(signals);
            stages.push_back(flags);

            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = static_cast<u32>(waits.size());
            info.pWaitSemaphores =
                waits.empty() ? nullptr : wait[wait.size() - 1].data();
            info.pWaitDstStageMask =
                waits.empty() ? nullptr : stages[stages.size() - 1].data();
            info.commandBufferCount = static_cast<u32>(buffers.size());
            info.pCommandBuffers =
                buffers.empty() ? nullptr : buffers[buffers.size() - 1].data();
            info.signalSemaphoreCount = static_cast<u32>(signals.size());
            info.pSignalSemaphores =
                signals.empty() ? nullptr : signal[signal.size() - 1].data();

            infos.push_back(info);
        }

        vkQueueSubmit(queue, static_cast<u32>(infos.size()), infos.data(),
                      fence == nullptr ? VK_NULL_HANDLE : cast<const VulkanFence*>(fence)->fence);
    }

    void VulkanQueue::present(const PresentInfo& presentInfo) const
    {
        vector<VkSemaphore> waits;
        VkSwapchainKHR swapchain =
            cast<VulkanSwapchain*>(presentInfo.swapchain)->swapchain;
        u32 index = presentInfo.image;

        for (const auto& sem : presentInfo.waits)
        {
            waits.push_back(cast<VulkanSemaphore*>(sem)->semaphore);
        }

        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = static_cast<u32>(waits.size());
        info.pWaitSemaphores = waits.data();
        info.swapchainCount = 1;
        info.pSwapchains = &swapchain;
        info.pImageIndices = &index;

        vkQueuePresentKHR(queue, &info);
    }
} // namespace helios