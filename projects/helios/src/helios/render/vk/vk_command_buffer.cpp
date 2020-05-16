#include <helios/render/vk/vk_command_buffer.hpp>

#include <helios/render/vk/vk_command_pool.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_framebuffer.hpp>
#include <helios/render/vk/vk_graphics_pipeline.hpp>
#include <helios/render/vk/vk_render_pass.hpp>
#include <helios/utility.hpp>

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
        beginInfo.clearValueCount = static_cast<uint32_t>(values.size());
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

    void VulkanCommandBuffer::draw(const uint32_t vertices,
                                   const uint32_t instances,
                                   const uint32_t baseVertex,
                                   const uint32_t baseInstance)
    {
        vkCmdDraw(buffer, vertices, instances, baseVertex, baseInstance);
    }

    void VulkanCommandBuffer::bind(const IGraphicsPipeline* pipeline)
    {
        vkCmdBindPipeline(
            buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            cast<const VulkanGraphicsPipeline*>(pipeline)->pipeline);
    }
} // namespace helios