#include <helios/render/vk/vk_descriptor_set.hpp>

#include <helios/render/vk/vk_buffer.hpp>
#include <helios/render/vk/vk_descriptor_pool.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_image_view.hpp>
#include <helios/render/vk/vk_sampler.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    VulkanDescriptorSet::~VulkanDescriptorSet()
    {
        if (!destroyed)
        {
            destroyed = true;
            if (!pool->destroyed)
            {
                pool->sets.erase(std::find(pool->sets.begin(), pool->sets.end(), this));
                if (pool->sets.empty())
                {
                    pool->reset();
                }
            }
        }
    }

    void VulkanDescriptorSet::write(const vector<DescriptorWriteInfo>& descriptors)
    {
        vector<VkWriteDescriptorSet> writes;
        vector<vector<VkDescriptorBufferInfo>> buffers;
        vector<vector<VkDescriptorImageInfo>> images;

        buffers.reserve(descriptors.size());
        images.reserve(descriptors.size());

        for (const auto& desc : descriptors)
        {
            vector<VkDescriptorImageInfo> imagesInfos;
            vector<VkDescriptorBufferInfo> buffersInfos;
            VkWriteDescriptorSet writeSet = {};

            switch (desc.type)
            {
            case EDescriptorType::SAMPLER: {
                for (const auto& info : std::get<vector<DescriptorImageInfo>>(desc.descriptorInfos))
                {
                    VkDescriptorImageInfo image;
                    image.sampler = cast<VulkanSampler*>(info.sampler)->sampler;
                    imagesInfos.push_back(image);
                }
                writeSet.descriptorCount = static_cast<u32>(imagesInfos.size());

                break;
            }
            case EDescriptorType::COMBINED_IMAGE_SAMPLER: {
                for (const auto& info : std::get<vector<DescriptorImageInfo>>(desc.descriptorInfos))
                {
                    VkDescriptorImageInfo image;
                    image.imageLayout = static_cast<VkImageLayout>(info.layout);
                    image.imageView = cast<VulkanImageView*>(info.view)->view;
                    image.sampler = cast<VulkanSampler*>(info.sampler)->sampler;
                    imagesInfos.push_back(image);
                }
                writeSet.descriptorCount = static_cast<u32>(imagesInfos.size());

                break;
            }
            case EDescriptorType::SAMPLED_IMAGE:
                [[fallthrough]];
            case EDescriptorType::STORAGE_IMAGE:
                [[fallthrough]];
            case EDescriptorType::INPUT_ATTACHMENT: {
                for (const auto& info : std::get<vector<DescriptorImageInfo>>(desc.descriptorInfos))
                {
                    VkDescriptorImageInfo image;
                    image.imageLayout = static_cast<VkImageLayout>(info.layout);
                    image.imageView = cast<VulkanImageView*>(info.view)->view;
                    imagesInfos.push_back(image);
                }
                writeSet.descriptorCount = static_cast<u32>(imagesInfos.size());

                break;
            }
            case EDescriptorType::UNIFORM_BUFFER:
                [[fallthrough]];
            case EDescriptorType::STORAGE_BUFFER:
                [[fallthrough]];
            case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
                [[fallthrough]];
            case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
                for (const auto& info : std::get<vector<DescriptorBufferInfo>>(desc.descriptorInfos))
                {
                    VkDescriptorBufferInfo buffer;
                    buffer.buffer = cast<VulkanBuffer*>(info.buffer)->buf;
                    buffer.offset = info.offset;
                    buffer.range = info.range;
                    buffersInfos.push_back(buffer);
                }
                writeSet.descriptorCount = static_cast<u32>(buffersInfos.size());

                break;
            }
            default:
                // TODO: Handle Texel Buffers
                break;
            };

            writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet.dstSet = set;
            writeSet.dstBinding = desc.binding;
            writeSet.dstArrayElement = desc.element;
            writeSet.descriptorType = static_cast<VkDescriptorType>(desc.type);
            if (!imagesInfos.empty())
            {
                images.push_back(helios::move(imagesInfos));
                writeSet.pImageInfo = images.back().data();
            }
            else if (!buffersInfos.empty())
            {
                buffers.push_back(helios::move(buffersInfos));
                writeSet.pBufferInfo = buffers.back().data();
            }
            writes.push_back(writeSet);
        }

        if (!writes.empty())
        {
            vkUpdateDescriptorSets(pool->device->device, static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
        }
    }
} // namespace helios