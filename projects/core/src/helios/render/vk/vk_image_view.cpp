#include <helios/render/vk/vk_image_view.hpp>

#include <helios/render/bldr/image_view_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_image.hpp>

#include <glad/vulkan.h>

#include <algorithm>
#include <cstdint>

namespace helios
{
    ImageViewBuilder::ImageViewBuilder()
    {
        _impl = new ImageViewBuilderImpl;
    }

    ImageViewBuilder::~ImageViewBuilder()
    {
        delete _impl;
    }

    ImageViewBuilder& ImageViewBuilder::type(const EImageViewType type)
    {
        _impl->type = type;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::image(const IImage* image)
    {
        _impl->image = const_cast<IImage*>(image);
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::format(const EFormat format)
    {
        _impl->format = format;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::redMapping(const EComponentSwizzle red)
    {
        _impl->red = red;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::greenMapping(
        const EComponentSwizzle green)
    {
        _impl->green = green;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::blueMapping(
        const EComponentSwizzle blue)
    {
        _impl->blue = blue;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::alphaMapping(
        const EComponentSwizzle alpha)
    {
        _impl->alpha = alpha;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::aspect(const EImageAspectFlags aspect)
    {
        _impl->aspect = aspect;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::baseMipLevel(const u32 base)
    {
        _impl->baseMip = base;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::mipLevels(const u32 count)
    {
        _impl->mipLevels = count;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::baseArrayLayer(const u32 base)
    {
        _impl->baseArrayLayer = base;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::arrayLayers(const u32 count)
    {
        _impl->arrayLayers = count;
        return *this;
    }

    IImageView* ImageViewBuilder::build() const
    {
        const auto image = cast<VulkanImage*>(_impl->image);

        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = image->image;
        info.viewType = static_cast<VkImageViewType>(_impl->type);
        info.format = static_cast<VkFormat>(_impl->format);
        info.components = {static_cast<VkComponentSwizzle>(_impl->red),
                           static_cast<VkComponentSwizzle>(_impl->green),
                           static_cast<VkComponentSwizzle>(_impl->blue),
                           static_cast<VkComponentSwizzle>(_impl->alpha)};
        info.subresourceRange = {static_cast<VkImageAspectFlags>(_impl->aspect),
                                 _impl->baseMip, _impl->mipLevels,
                                 _impl->baseArrayLayer, _impl->arrayLayers};

        VulkanImageView* view = new VulkanImageView;
        view->image = image;

        vkCreateImageView(image->device->device, &info, nullptr, &view->view);

        image->views.push_back(view);

        return view;
    }

    VulkanImageView::~VulkanImageView()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!image->destroyed)
            {
                image->views.erase(
                    std::find(image->views.begin(), image->views.end(), this));
            }
            vkDestroyImageView(image->device->device, view, nullptr);
        }
    }
} // namespace helios