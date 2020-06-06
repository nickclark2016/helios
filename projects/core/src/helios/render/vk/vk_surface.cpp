#include <helios/render/vk/vk_surface.hpp>

#include <helios/render/bldr/surface_builder_impl.hpp>
#include <helios/render/vk/vk_context.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_physical_device.hpp>
#include <helios/render/vk/vk_swapchain.hpp>

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

#include <algorithm>

namespace helios
{
    extern GLFWwindow* as_native(const IWindow* window);

    SurfaceBuilder::SurfaceBuilder()
    {
        _impl = new SurfaceBuilderImpl;
    }

    SurfaceBuilder::~SurfaceBuilder()
    {
        delete _impl;
    }

    SurfaceBuilder& SurfaceBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    SurfaceBuilder& SurfaceBuilder::window(const IWindow* window)
    {
        _impl->window = const_cast<IWindow*>(window);
        return *this;
    }

    ISurface* SurfaceBuilder::build() const
    {
        VulkanSurface* surface = new VulkanSurface;
        surface->device = cast<VulkanDevice*>(_impl->device);
        surface->context =
            cast<VulkanContext*>(surface->device->parent->context);
        surface->device->surfaces.push_back(surface);

        GLFWwindow* win = as_native(_impl->window);

        VkSurfaceKHR surf;
        glfwCreateWindowSurface(surface->context->instance, win, nullptr,
                                &surf);
        surface->surface = surf;

        return surface;
    }

    VulkanSurface::~VulkanSurface()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->surfaces.erase(std::find(device->surfaces.begin(),
                                                 device->surfaces.end(), this));
            }

            if (!swapchain->destroyed)
            {
                delete swapchain;
            }
            vkDestroySurfaceKHR(context->instance, surface, nullptr);
        }
    }

    IWindow* VulkanSurface::window() const
    {
        return windowPtr;
    }

    ISurface::SwapchainSupport VulkanSurface::swapchainSupport(
        const IPhysicalDevice* device) const
    {
        const auto d = cast<const VulkanPhysicalDevice*>(device);

        VkSurfaceCapabilitiesKHR surfaceCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(d->device, surface,
                                                  &surfaceCaps);

        vector<SurfaceFormat> formats;
        vector<EPresentMode> presentModes;

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(d->device, surface,
                                                  &presentModeCount, nullptr);
        vector<VkPresentModeKHR> vkPresentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            d->device, surface, &presentModeCount, vkPresentModes.data());

        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(d->device, surface, &formatCount,
                                             nullptr);
        vector<VkSurfaceFormatKHR> surfFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(d->device, surface, &formatCount,
                                             surfFormats.data());
        for (const auto format : surfFormats)
        {
            formats.push_back({static_cast<EFormat>(format.format),
                               static_cast<EColorSpace>(format.colorSpace)});
        }

        for (const auto& mode : vkPresentModes)
        {
            presentModes.push_back(static_cast<EPresentMode>(mode));
        }

        return {surfaceCaps.minImageCount,
                surfaceCaps.maxImageCount,
                surfaceCaps.currentExtent.width,
                surfaceCaps.currentExtent.height,
                surfaceCaps.minImageExtent.width,
                surfaceCaps.minImageExtent.height,
                surfaceCaps.maxImageExtent.width,
                surfaceCaps.maxImageExtent.height,
                surfaceCaps.maxImageArrayLayers,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != VK_FALSE,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) != VK_FALSE,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) != VK_FALSE,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != VK_FALSE,
                surfaceCaps.supportedUsageFlags,
                static_cast<ESurfaceTransformFlagBits>(
                    surfaceCaps.currentTransform),
                surfaceCaps.supportedTransforms,
                formats,
                presentModes};
    }
} // namespace helios