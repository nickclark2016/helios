#include <helios/render/vk/vk_command_buffer.hpp>

#include <helios/containers/utility.hpp>
#include <helios/render/vk/vk_buffer.hpp>
#include <helios/render/vk/vk_command_pool.hpp>
#include <helios/render/vk/vk_descriptor_set.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_framebuffer.hpp>
#include <helios/render/vk/vk_graphics_pipeline.hpp>
#include <helios/render/vk/vk_image.hpp>
#include <helios/render/vk/vk_pipeline_layout.hpp>
#include <helios/render/vk/vk_render_pass.hpp>

#include <glad/vulkan.h>

#include <algorithm>
#include <cstring>

namespace helios
{
    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!pool->destroyed)
            {
                pool->buffers.erase(std::find(pool->buffers.begin(),
                                              pool->buffers.end(), this));
            }

            vkFreeCommandBuffers(pool->device->device, pool->pool, 1, &buffer);
        }
    }

    void VulkanCommandBuffer::record() const
    {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        vkBeginCommandBuffer(buffer, &info);
    }

    void VulkanCommandBuffer::end() const
    {
        vkEndCommandBuffer(buffer);
    }

    void VulkanCommandBuffer::beginRenderPass(const RenderPassRecordInfo& info,
                                              const bool isInline)
    {
        vector<VkClearValue> values;
        for (const auto& value : info.clearValues)
        {
            VkClearValue v;
            memcpy(&v, &value, sizeof(VkClearValue));
            values.push_back(v);
        }

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.framebuffer = cast<VulkanFramebuffer*>(info.renderTarget)->fb;
        beginInfo.renderArea = {{info.x, info.y}, {info.width, info.height}};
        beginInfo.renderPass =
            cast<VulkanRenderPass*>(info.renderpass)->renderpass;
        beginInfo.clearValueCount = static_cast<u32>(values.size());
        beginInfo.pClearValues = values.data();

        vkCmdBeginRenderPass(
            buffer, &beginInfo,
            isInline ? VK_SUBPASS_CONTENTS_INLINE
                     : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(buffer);
    }

    void VulkanCommandBuffer::draw(const u32 vertices, const u32 instances,
                                   const u32 baseVertex, const u32 baseInstance)
    {
        vkCmdDraw(buffer, vertices, instances, baseVertex, baseInstance);
    }

    void VulkanCommandBuffer::draw(const u32 indexCount, const u32 instances,
                                   const u32 baseIndex, const i32 vertexOffset,
                                   const u32 baseInstance)
    {
        vkCmdDrawIndexed(buffer, indexCount, instances, baseIndex, vertexOffset,
                         baseInstance);
    }

    void VulkanCommandBuffer::bind(const IGraphicsPipeline* pipeline)
    {
        vkCmdBindPipeline(
            buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            cast<const VulkanGraphicsPipeline*>(pipeline)->pipeline);
    }

    void VulkanCommandBuffer::bind(const vector<IBuffer*>& buffers,
                                   const vector<u64>& offsets, u32 first)
    {
        vector<VkBuffer> bufs;
        vector<VkDeviceSize> offs;
        bufs.reserve(buffers.size());
        offs.reserve(buffers.size());

        for (const auto buf : buffers)
        {
            bufs.push_back(cast<VulkanBuffer*>(buf)->buf);
        }

        for (const auto off : offsets)
        {
            offs.push_back(off);
        }

        vkCmdBindVertexBuffers(buffer, first, static_cast<u32>(bufs.size()),
                               bufs.data(), offs.data());
    }

    void VulkanCommandBuffer::bind(const vector<IDescriptorSet*> descriptorSets,
                                   const IGraphicsPipeline* pipeline, u32 first)
    {
        vector<VkDescriptorSet> sets;
        for (const auto set : descriptorSets)
        {
            sets.push_back(cast<VulkanDescriptorSet*>(set)->set);
        }

        vkCmdBindDescriptorSets(
            buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            cast<const VulkanGraphicsPipeline*>(pipeline)->layout->layout,
            first, static_cast<u32>(sets.size()), sets.data(), 0, nullptr);
    }

    void VulkanCommandBuffer::bind(IBuffer* elements, u64 offset)
    {
        vkCmdBindIndexBuffer(buffer, cast<VulkanBuffer*>(elements)->buf, offset,
                             VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandBuffer::copy(IBuffer* src, IBuffer* dst,
                                   const vector<BufferCopyRegion>& regions)
    {
        vector<VkBufferCopy> copies;
        for (const auto& region : regions)
        {
            copies.push_back({region.srcOffset, region.dstOffset, region.size});
        }

        vkCmdCopyBuffer(buffer, cast<VulkanBuffer*>(src)->buf,
                        cast<VulkanBuffer*>(dst)->buf,
                        static_cast<u32>(copies.size()), copies.data());
    }

    void VulkanCommandBuffer::copy(IBuffer* src, IImage* dst,
                                   const vector<BufferImageCopyRegion>& regions,
                                   const EImageLayout format)
    {
        vector<VkBufferImageCopy> copies;
        for (const auto& region : regions)
        {
            copies.push_back({region.offset,
                              region.imageWidth,
                              region.imageHeight,
                              {region.aspect, region.mipLevel,
                               region.arrayLayer, region.layerCount},
                              {region.x, region.y, region.z},
                              {region.width, region.height, region.depth}});
        }
        vkCmdCopyBufferToImage(buffer, cast<VulkanBuffer*>(src)->buf,
                               cast<VulkanImage*>(dst)->image,
                               static_cast<VkImageLayout>(format),
                               static_cast<u32>(copies.size()), copies.data());
    }

    void VulkanCommandBuffer::barrier(
        EPipelineStageFlags src, EPipelineStageFlags dst,
        EDependencyFlags dependency,
        const vector<BufferMemoryBarrier>& bufferBarriers,
        const vector<ImageMemoryBarrier>& imageBarriers)
    {
        vector<VkImageMemoryBarrier> images;
        for (const auto& image : imageBarriers)
        {
            images.push_back(
                {
                    VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    nullptr,
                    static_cast<VkAccessFlags>(image.srcAccess),
                    static_cast<VkAccessFlags>(image.dstAccess),
                    static_cast<VkImageLayout>(image.oldLayout),
                    static_cast<VkImageLayout>(image.newLayout),
                    image.srcQueueFamilyIndex,
                    image.dstQueueFamilyIndex,
                    cast<VulkanImage*>(image.image)->image,
                    {
                        static_cast<VkImageAspectFlags>(image.aspect),
                        image.mipLevel,
                        image.mipCount,
                        image.arrayLayer,
                        image.layerCount
                    }
                });
        }

        vector<VkBufferMemoryBarrier> buffers;
        for (const auto& buffer : bufferBarriers)
        {
            buffers.push_back(
                {
                    VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    nullptr,
                    static_cast<VkAccessFlags>(buffer.srcAccess),
                    static_cast<VkAccessFlags>(buffer.dstAccess),
                    buffer.srcQueueFamilyIndex,
                    buffer.dstQueueFamilyIndex,
                    cast<VulkanBuffer*>(buffer.buffer)->buf,
                    buffer.offset,
                    buffer.size
                });
        }

        vkCmdPipelineBarrier(buffer, static_cast<VkPipelineStageFlags>(src),
                             static_cast<VkPipelineStageFlags>(dst),
                             static_cast<VkDependencyFlags>(dependency), 0,
                             nullptr, static_cast<u32>(buffers.size()), buffers.data(),
                             static_cast<u32>(images.size()), images.data());
    }
} // namespace helios