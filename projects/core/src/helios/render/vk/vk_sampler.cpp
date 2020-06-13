#include <helios/render/vk/vk_sampler.hpp>

#include <helios/render/bldr/sampler_builder_impl.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    SamplerBuilder::SamplerBuilder()
    {
        _impl = new SamplerBuilderImpl;
    }

    SamplerBuilder::~SamplerBuilder()
    {
        delete _impl;
    }

    SamplerBuilder& SamplerBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    SamplerBuilder& SamplerBuilder::magnification(const EFilter filter)
    {
        _impl->magnification = filter;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::minification(const EFilter filter)
    {
        _impl->minification = filter;
        return *this;
    };

    SamplerBuilder& SamplerBuilder::mipmap(const ESamplerMipMapMode mode)
    {
        _impl->mip = mode;
        return *this;
    };

    SamplerBuilder& SamplerBuilder::addressModeU(const ESamplerAddressMode mode)
    {
        _impl->addrU = mode;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::addressModeV(const ESamplerAddressMode mode)
    {
        _impl->addrV = mode;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::addressModeW(const ESamplerAddressMode mode)
    {
        _impl->addrW = mode;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::mipLodBias(const f32 bias)
    {
        _impl->mipLodBias = bias;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::anisotropy(const f32 max)
    {
        _impl->anisoEnable = true;
        _impl->maxAniso = max;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::compare(const ECompareOp op)
    {
        _impl->compareEnable = true;
        _impl->compare = op;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::minLod(const f32 lod)
    {
        _impl->minLod = lod;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::maxLod(const f32 lod)
    {
        _impl->maxLod = lod;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::border(const EBorderColor color)
    {
        _impl->border = color;
        return *this;
    }

    SamplerBuilder& SamplerBuilder::unnormalized(const bool unnormalized)
    {
        _impl->unnormalized = unnormalized;
        return *this;
    }

    ISampler* SamplerBuilder::build() const
    {
        VulkanSampler* sampler = new VulkanSampler;
        sampler->device = cast<VulkanDevice*>(_impl->device);

        VkSamplerCreateInfo create = {};
        create.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        create.pNext = nullptr;
        create.flags = 0;
        create.magFilter = static_cast<VkFilter>(_impl->magnification);
        create.minFilter = static_cast<VkFilter>(_impl->minification);
        create.mipmapMode = static_cast<VkSamplerMipmapMode>(_impl->mip);
        create.addressModeU = static_cast<VkSamplerAddressMode>(_impl->addrU);
        create.addressModeV = static_cast<VkSamplerAddressMode>(_impl->addrV);
        create.addressModeW = static_cast<VkSamplerAddressMode>(_impl->addrW);
        create.mipLodBias = _impl->mipLodBias ? VK_TRUE : VK_FALSE;
        create.anisotropyEnable = _impl->anisoEnable;
        if (create.anisotropyEnable)
        {
            create.maxAnisotropy = _impl->maxAniso;
        }
        create.compareEnable = _impl->compareEnable ? VK_TRUE : VK_FALSE;
        if (create.compareEnable)
        {
            create.compareOp = static_cast<VkCompareOp>(_impl->compare);
        }
        create.minLod = _impl->minLod;
        create.maxLod = _impl->maxLod;
        create.borderColor = static_cast<VkBorderColor>(_impl->border);
        create.unnormalizedCoordinates =
            _impl->unnormalized ? VK_TRUE : VK_FALSE;

        vkCreateSampler(sampler->device->device, &create, nullptr,
                        &sampler->sampler);
        sampler->device->samplers.push_back(sampler);

        return sampler;
    }

    VulkanSampler::~VulkanSampler()
    {
        if (!destroyed)
        {
            destroyed = true;

            vkDestroySampler(device->device, sampler, nullptr);

            if (!device->destroyed)
            {
                device->samplers.erase(std::find(device->samplers.begin(),
                                                 device->samplers.end(), this));
            }
        }
    }
}; // namespace helios
