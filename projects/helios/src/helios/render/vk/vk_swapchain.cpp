#include <helios/render/vk/vk_swapchain.hpp>

#include <helios/containers/vector.hpp>
#include <helios/render/bldr/swapchain_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_fence.hpp>
#include <helios/render/vk/vk_image.hpp>
#include <helios/render/vk/vk_semaphore.hpp>
#include <helios/render/vk/vk_surface.hpp>
#include <helios/utility.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    SwapchainBuilder::SwapchainBuilder()
    {
        _impl = new SwapchainBuilderImpl;
    }

    SwapchainBuilder::~SwapchainBuilder()
    {
        delete _impl;
    }

    SwapchainBuilder& SwapchainBuilder::surface(const ISurface* surface)
    {
        _impl->surface = const_cast<ISurface*>(surface);
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::images(const u32 count)
    {
        _impl->count = count;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::width(const u32 width)
    {
        _impl->width = width;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::height(const u32 height)
    {
        _impl->height = height;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::layers(const u32 layers)
    {
        _impl->layers = layers;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::previous(const ISwapchain* previous)
    {
        _impl->previous = const_cast<ISwapchain*>(previous);
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::present(const EPresentMode present)
    {
        _impl->presentMode = present;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::format(const EFormat format)
    {
        _impl->format = format;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::colorSpace(const EColorSpace colorSpace)
    {
        _impl->colorSpace = colorSpace;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::queues(const vector<IQueue*>& queues)
    {
        _impl->queues = queues;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::usage(const EImageUsageFlags usage)
    {
        _impl->usage = usage;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::transform(
        const ESurfaceTransformFlagBits transform)
    {
        _impl->transform = transform;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaOpaque()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaPremultiply()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaPostmultiply()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaInherit()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::clipped()
    {
        _impl->clipped = VK_TRUE;
        return *this;
    }

    ISwapchain* SwapchainBuilder::build() const
    {
        VulkanSwapchain* result = new VulkanSwapchain;

        vector<u32> queueFamilies;
        for (const auto& queue : _impl->queues)
        {
            const u32 index = queue->props().index;
            if (std::find(queueFamilies.begin(), queueFamilies.end(), index) ==
                queueFamilies.end())
            {
                queueFamilies.push_back(index);
            }
        }

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.minImageCount = _impl->count;
        info.imageFormat = static_cast<VkFormat>(_impl->format);
        info.imageColorSpace = static_cast<VkColorSpaceKHR>(_impl->colorSpace);
        info.surface = cast<VulkanSurface*>(_impl->surface)->surface;
        info.imageExtent = {_impl->width, _impl->height};
        info.imageArrayLayers = _impl->layers;
        info.imageUsage = _impl->usage;
        info.imageSharingMode = queueFamilies.size() > 1
                                    ? VK_SHARING_MODE_CONCURRENT
                                    : VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = static_cast<u32>(queueFamilies.size());
        info.pQueueFamilyIndices = queueFamilies.data();
        info.preTransform =
            static_cast<VkSurfaceTransformFlagBitsKHR>(_impl->transform);
        info.compositeAlpha =
            static_cast<VkCompositeAlphaFlagBitsKHR>(_impl->alpha);
        info.clipped = _impl->clipped ? VK_TRUE : VK_FALSE;
        info.oldSwapchain =
            _impl->previous ? cast<VulkanSwapchain*>(_impl->previous)->swapchain
                            : VK_NULL_HANDLE;
        info.presentMode = static_cast<VkPresentModeKHR>(_impl->presentMode);

        VulkanDevice* device =
            cast<VulkanDevice*>(cast<VulkanSurface*>(_impl->surface)->device);

        VkSwapchainKHR swapchain;
        vkCreateSwapchainKHR(device->device, &info, nullptr, &swapchain);
        result->swapchain = swapchain;
        result->device = cast<VulkanDevice*>(device);

        u32 imageCount;
        vkGetSwapchainImagesKHR(result->device->device, result->swapchain,
                                &imageCount, nullptr);
        vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(result->device->device, result->swapchain,
                                &imageCount, images.data());

        vector<IImageView*> views;
        for (const auto image : images)
        {
            VulkanImage* img = new VulkanImage;
            img->image = image;
            img->device = result->device;
            img->owned = false;
            device->images.push_back(img);

            const auto view = ImageViewBuilder()
                                  .type(EImageViewType::TYPE_2D)
                                  .image(img)
                                  .aspect(ASPECT_COLOR)
                                  .format(_impl->format)
                                  .build();
            views.push_back(view);
        }
        result->imgViews = views;
        result->fmt = _impl->format;
        result->surface = cast<VulkanSurface*>(_impl->surface);
        result->surface->swapchain = result;

        return result;
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        if (!destroyed)
        {
            destroyed = true;

            imgViews.clear();
            if (!surface->destroyed)
            {
                delete surface;
            }
            vkDestroySwapchainKHR(device->device, swapchain, nullptr);
            surface->swapchain = nullptr;
        }
    }

    u32 VulkanSwapchain::imagesCount() const
    {
        return static_cast<u32>(imgViews.size());
    }

    vector<IImageView*> VulkanSwapchain::views() const
    {
        return imgViews;
    }

    EFormat VulkanSwapchain::format() const
    {
        return fmt;
    }

    u32 VulkanSwapchain::acquireNextImage(const uint64_t wait,
                                          const ISemaphore* signal,
                                          const IFence* fence)
    {
        const VulkanSemaphore* vkSem = cast<const VulkanSemaphore*>(signal);
        const VkSemaphore sem = vkSem ? vkSem->semaphore : VK_NULL_HANDLE;

        const VulkanFence* vkFence = cast<const VulkanFence*>(fence);
        const VkFence fen = vkFence ? vkFence->fence : VK_NULL_HANDLE;

        u32 index = UINT32_MAX;
        vkAcquireNextImageKHR(device->device, swapchain, wait, sem, fen,
                              &index);
        return index;
    }
} // namespace helios