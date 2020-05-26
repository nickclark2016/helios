#include <helios/render/vk/vk_descriptor_pool.hpp>

#include <helios/render/bldr/descriptor_pool_builder_impl.hpp>
#include <helios/render/vk/vk_descriptor_set.hpp>
#include <helios/render/vk/vk_descriptor_set_layout.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

namespace helios
{
    DescriptorPoolBuilder::DescriptorPoolBuilder()
    {
        _impl = new DescriptorPoolBuilderImpl;
    }

    DescriptorPoolBuilder::~DescriptorPoolBuilder()
    {
        delete _impl;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::maxSetCount(const u32 sets)
    {
        _impl->sets = sets;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::samplers(const u32 count)
    {
        _impl->samplerCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::imageSamplers(const u32 count)
    {
        _impl->imageSamplerCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::sampledImages(const u32 count)
    {
        _impl->sampledImageCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::storageImages(const u32 count)
    {
        _impl->storageImageCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::uniformTexelBuffers(
        const u32 count)
    {
        _impl->uniformTexelCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::storageTexelBuffers(
        const u32 count)
    {
        _impl->storageTexelCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::uniformBuffers(
        const u32 count)
    {
        _impl->uniformCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::storageBuffers(
        const u32 count)
    {
        _impl->storageCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::dynamicUniformBuffers(
        const u32 count)
    {
        _impl->uniformDynamicCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::dynamicStorageBuffers(
        const u32 count)
    {
        _impl->storageDynamicCount = count;
        return *this;
    }

    DescriptorPoolBuilder& DescriptorPoolBuilder::inputAttachments(
        const u32 count)
    {
        _impl->inputAttachmentCount = count;
        return *this;
    }

    IDescriptorPool* DescriptorPoolBuilder::build() const
    {
        VulkanDescriptorPool* pool = new VulkanDescriptorPool;

        pool->device = cast<VulkanDevice*>(_impl->device);
        pool->maxSetCount = _impl->sets;
        pool->allocatedSetCount = 0;
        pool->maxSamplerCount = _impl->samplerCount;
        pool->allocatedSamplerCount = 0;
        pool->maxImageSamplerCount = _impl->imageSamplerCount;
        pool->allocatedImageSamplerCount = 0;
        pool->maxSampledImageCount = _impl->sampledImageCount;
        pool->allocatedSampledImageCount = 0;
        pool->maxStorageImageCount = _impl->storageImageCount;
        pool->allocatedStorageImageCount = 0;
        pool->maxUniformTexelBufferCount = _impl->uniformTexelCount;
        pool->allocatedUniformTexelBufferCount = 0;
        pool->maxStorageTexelBufferCount = _impl->storageTexelCount;
        pool->allocatedStorageTexelBufferCount = 0;
        pool->maxUniformBufferCount = _impl->uniformCount;
        pool->allocatedUniformBufferCount = 0;
        pool->maxStorageBufferCount = _impl->storageCount;
        pool->allocatedStorageBufferCount = 0;
        pool->maxUniformDynamicCount = _impl->uniformDynamicCount;
        pool->allocatedUniformDynamicCount = 0;
        pool->maxStorageDynamicCount = _impl->storageDynamicCount;
        pool->allocatedStorageDynamicCount = 0;
        pool->maxInputAttachmentCount = _impl->inputAttachmentCount;
        pool->allocatedInputAttachmentCount = 0;

        vector<VkDescriptorPoolSize> descs;
        if (pool->maxSamplerCount > 0)
        {
            descs.push_back(
                {VK_DESCRIPTOR_TYPE_SAMPLER, pool->maxSamplerCount});
        }

        if (pool->maxImageSamplerCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                             pool->maxImageSamplerCount});
        }

        if (pool->maxSampledImageCount > 0)
        {
            descs.push_back(
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, pool->maxSampledImageCount});
        }

        if (pool->maxStorageImageCount > 0)
        {
            descs.push_back(
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, pool->maxStorageImageCount});
        }

        if (pool->maxUniformTexelBufferCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
                             pool->maxUniformTexelBufferCount});
        }

        if (pool->maxStorageTexelBufferCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
                             pool->maxStorageTexelBufferCount});
        }

        if (pool->maxUniformBufferCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             pool->maxUniformBufferCount});
        }

        if (pool->maxStorageBufferCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                             pool->maxStorageBufferCount});
        }

        if (pool->maxUniformDynamicCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                             pool->maxUniformDynamicCount});
        }

        if (pool->maxStorageDynamicCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
                             pool->maxStorageDynamicCount});
        }

        if (pool->maxInputAttachmentCount > 0)
        {
            descs.push_back({VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                             pool->maxInputAttachmentCount});
        }

        VkDescriptorPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = pool->maxSetCount;
        createInfo.poolSizeCount = static_cast<u32>(descs.size());
        createInfo.pPoolSizes = descs.data();
        vkCreateDescriptorPool(pool->device->device, &createInfo, nullptr,
                               &pool->pool);

        return pool;
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        if (!destroyed)
        {
            destroyed = true;
            device->descriptorPools.erase(
                std::find(device->descriptorPools.begin(),
                          device->descriptorPools.end(), this));
            vkDestroyDescriptorPool(device->device, pool, nullptr);
        }
    }

    vector<IDescriptorSet*> VulkanDescriptorPool::allocate(
        const vector<IDescriptorSetLayout*>& layouts)
    {
        if (layouts.empty())
        {
            return {};
        }

        vector<VkDescriptorSet> vkDescriptorSets;
        vector<IDescriptorSet*> descriptorSets;

        vector<VkDescriptorSetLayout> setLayouts;

        for (const auto& layout : layouts)
        {
            VulkanDescriptorSetLayout* setLayout =
                cast<VulkanDescriptorSetLayout*>(layout);
            setLayouts.push_back(setLayout->layout);

            for (const auto& binding : layout->bindings())
            {
                switch (binding.type)
                {
                case EDescriptorType::SAMPLER: {
                    allocatedSamplerCount += binding.count;
                    break;
                }
                case EDescriptorType::COMBINED_IMAGE_SAMPLER: {
                    allocatedImageSamplerCount += binding.count;
                    break;
                }
                case EDescriptorType::SAMPLED_IMAGE: {
                    allocatedSampledImageCount += binding.count;
                    break;
                }
                case EDescriptorType::STORAGE_IMAGE: {
                    allocatedStorageImageCount += binding.count;
                    break;
                }
                case EDescriptorType::UNIFORM_TEXEL_BUFFER: {
                    allocatedUniformTexelBufferCount += binding.count;
                    break;
                }
                case EDescriptorType::STORAGE_TEXEL_BUFFER: {
                    allocatedStorageTexelBufferCount += binding.count;
                    break;
                }
                case EDescriptorType::UNIFORM_BUFFER: {
                    allocatedUniformBufferCount += binding.count;
                    break;
                }
                case EDescriptorType::STORAGE_BUFFER: {
                    allocatedStorageBufferCount += binding.count;
                    break;
                }
                case EDescriptorType::UNIFORM_BUFFER_DYNAMIC: {
                    allocatedUniformDynamicCount += binding.count;
                    break;
                }
                case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
                    allocatedStorageDynamicCount += binding.count;
                    break;
                }
                case EDescriptorType::INPUT_ATTACHMENT: {
                    allocatedInputAttachmentCount += binding.count;
                    break;
                }
                default:
                    break;
                }
            }
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = static_cast<u32>(setLayouts.size());
        allocInfo.pSetLayouts = setLayouts.data();

        return {};
    }

    void VulkanDescriptorPool::reset()
    {
        vkFreeDescriptorSets(device->device, pool,
                             static_cast<u32>(vkSets.size()), vkSets.data());
        vkSets.clear();

        for (auto set : sets)
        {
            delete set;
        }
        sets.clear();

        allocatedSetCount = 0;
        allocatedSamplerCount = 0;
        allocatedImageSamplerCount = 0;
        allocatedSampledImageCount = 0;
        allocatedStorageImageCount = 0;
        allocatedUniformTexelBufferCount = 0;
        allocatedStorageTexelBufferCount = 0;
        allocatedUniformBufferCount = 0;
        allocatedStorageBufferCount = 0;
        allocatedUniformDynamicCount = 0;
        allocatedStorageDynamicCount = 0;
        allocatedInputAttachmentCount = 0;
    }

    u32 VulkanDescriptorPool::maxSets() const
    {
        return maxSetCount;
    }

    u32 VulkanDescriptorPool::allocatedSets() const
    {
        return allocatedSetCount;
    }

    u32 VulkanDescriptorPool::maxSamplers() const
    {
        return maxSamplerCount;
    }

    u32 VulkanDescriptorPool::allocatedSamplers() const
    {
        return allocatedSamplerCount;
    }

    u32 VulkanDescriptorPool::maxCombinedImageSamplers() const
    {
        return maxImageSamplerCount;
    }

    u32 VulkanDescriptorPool::allocatedCombinedImageSamplers() const
    {
        return allocatedImageSamplerCount;
    }

    u32 VulkanDescriptorPool::maxSampledImages() const
    {
        return maxSampledImageCount;
    }

    u32 VulkanDescriptorPool::allocatedSampledImages() const
    {
        return allocatedSampledImageCount;
    }

    u32 VulkanDescriptorPool::maxStorageImages() const
    {
        return maxStorageImageCount;
    }

    u32 VulkanDescriptorPool::allocatedStorageImages() const
    {
        return allocatedStorageImageCount;
    }

    u32 VulkanDescriptorPool::maxUniformTexelBuffers() const
    {
        return maxUniformTexelBufferCount;
    }

    u32 VulkanDescriptorPool::allocatedUniformTexelBuffers() const
    {
        return allocatedUniformTexelBufferCount;
    }

    u32 VulkanDescriptorPool::maxStorageTexelBuffers() const
    {
        return maxStorageTexelBufferCount;
    }

    u32 VulkanDescriptorPool::allocatedStorageTexelBuffers() const
    {
        return allocatedStorageTexelBufferCount;
    }

    u32 VulkanDescriptorPool::maxUniformBuffers() const
    {
        return maxUniformBufferCount;
    }

    u32 VulkanDescriptorPool::allocatedUniformBuffers() const
    {
        return allocatedUniformBufferCount;
    }

    u32 VulkanDescriptorPool::maxStorageBuffers() const
    {
        return maxStorageBufferCount;
    }

    u32 VulkanDescriptorPool::allocatedStorageBuffers() const
    {
        return allocatedStorageBufferCount;
    }

    u32 VulkanDescriptorPool::maxDynamicUniformBuffers() const
    {
        return maxUniformDynamicCount;
    }

    u32 VulkanDescriptorPool::allocatedDynamicUniformBuffers() const
    {
        return allocatedUniformDynamicCount;
    }

    u32 VulkanDescriptorPool::maxDynamicStorageBuffers() const
    {
        return maxStorageDynamicCount;
    }

    u32 VulkanDescriptorPool::allocatedDynamicStorageBuffers() const
    {
        return allocatedStorageDynamicCount;
    }

    u32 VulkanDescriptorPool::maxInputAttachments() const
    {
        return maxInputAttachmentCount;
    }

    u32 VulkanDescriptorPool::allocatedInputAttachments() const
    {
        return allocatedInputAttachmentCount;
    }
} // namespace helios