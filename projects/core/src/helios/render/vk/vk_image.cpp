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

    ImageBuilder& ImageBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    ImageBuilder& ImageBuilder::type(const EImageType type)
    {
        _impl->type = type;
        return *this;
    }

    ImageBuilder& ImageBuilder::format(const EFormat format)
    {
        _impl->format = format;
        return *this;
    }

    ImageBuilder& ImageBuilder::extent(const u32 width, const u32 height,
                                       const u32 depth)
    {
        _impl->width = width;
        _impl->height = height;
        _impl->depth = depth;
        return *this;
    }

    ImageBuilder& ImageBuilder::mipLevels(const u32 levels)
    {
        _impl->mipLevelCount = levels;
        return *this;
    }

    ImageBuilder& ImageBuilder::arrayLayers(const u32 layers)
    {
        _impl->arrayLayerCount = layers;
        return *this;
    }

    ImageBuilder& ImageBuilder::samples(const ESampleCountFlagBits samples)
    {
        _impl->samples = samples;
        return *this;
    }

    ImageBuilder& ImageBuilder::tiling(const EImageTiling tiling)
    {
        _impl->tiling = tiling;
        return *this;
    }

    ImageBuilder& ImageBuilder::usage(const EImageUsageFlags usage)
    {
        _impl->usage = usage;
        return *this;
    }

    ImageBuilder& ImageBuilder::queues(const vector<IQueue*>& queues)
    {
        _impl->queues = queues;
        return *this;
    }

    ImageBuilder& ImageBuilder::initialLayout(const EImageLayout layout)
    {
        _impl->layout = layout;
        return *this;
    }

    ImageBuilder& ImageBuilder::preferredFlags(const EMemoryPropertyFlags flags)
    {
        _impl->preferred = flags;
        return *this;
    }

    ImageBuilder& ImageBuilder::requiredFlags(const EMemoryPropertyFlags flags)
    {
        _impl->required = flags;
        return *this;
    }

    ImageBuilder& ImageBuilder::memoryUsage(const EMemoryUsage usage)
    {
        _impl->memUsage = usage;
        return *this;
    }

    IImage* ImageBuilder::build() const
    {
        VulkanImage* image = new VulkanImage;
        image->device = cast<VulkanDevice*>(_impl->device);

        vector<u32> queueFamilies;

        VkImageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType = static_cast<VkImageType>(_impl->type);
        createInfo.format = static_cast<VkFormat>(_impl->format);
        createInfo.extent = {_impl->width, _impl->height, _impl->depth};
        createInfo.mipLevels = _impl->mipLevelCount;
        createInfo.arrayLayers = _impl->arrayLayerCount;
        createInfo.samples = static_cast<VkSampleCountFlagBits>(_impl->samples);
        createInfo.tiling = static_cast<VkImageTiling>(_impl->tiling);
        createInfo.initialLayout = static_cast<VkImageLayout>(_impl->layout);
        createInfo.usage = _impl->usage;
        createInfo.sharingMode = _impl->queues.empty()
                                     ? VK_SHARING_MODE_EXCLUSIVE
                                     : VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;

        if (!_impl->queues.empty())
        {
            for (const auto& queue : _impl->queues)
            {
                const u32 family = queue->props().index;
                if (std::find(queueFamilies.begin(), queueFamilies.end(),
                              family) == queueFamilies.end())
                {
                    queueFamilies.push_back(family);
                }
            }

            createInfo.queueFamilyIndexCount =
                static_cast<u32>(queueFamilies.size());
            createInfo.pQueueFamilyIndices = queueFamilies.data();
        }

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.requiredFlags = _impl->required;
        allocInfo.preferredFlags = _impl->preferred;
        allocInfo.usage = static_cast<VmaMemoryUsage>(_impl->memUsage);

        vmaCreateImage(image->device->memAllocator, &createInfo, &allocInfo,
                       &image->image, &image->allocation, nullptr);

        image->device->images.push_back(image);
        image->owned = true;
        image->format = _impl->format;

        return image;
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
                vmaDestroyImage(device->memAllocator, image, allocation);
            }

            if (!device->destroyed)
            {
                device->images.erase(std::find(device->images.begin(),
                                               device->images.end(), this));
            }
        }
    }

    EFormat VulkanImage::getFormat() const noexcept
    {
        return format;
    }
} // namespace helios