#include <helios/render/vk/vk_image.hpp>

#include <helios/render/bldr/image_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_image_view.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    ImageBuilder::ImageBuilder()
    {
        _impl = new ImageBuilderImpl;
    }

    ImageBuilder::~ImageBuilder()
    {
        delete _impl;
    }

    VulkanImage::~VulkanImage()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (auto view : views)
            {
                delete view;
            }

            if (owned)
            {
                vkDestroyImage(device->device, image, nullptr);
            }

            if (!device->destroyed)
            {
                device->images.erase(std::find(device->images.begin(),
                                               device->images.end(), this));
            }
        }
    }
} // namespace helios