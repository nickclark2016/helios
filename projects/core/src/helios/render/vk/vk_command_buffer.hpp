#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <glad/vulkan.h>

namespace helios
{
    struct VulkanCommandPool;

    struct VulkanCommandBuffer final : ICommandBuffer
    {
        VulkanCommandBuffer() = default;
        ~VulkanCommandBuffer() override;

        void record() const override;
        void end() const override;
        void beginRenderPass(const RenderPassRecordInfo& info,
                             const bool isInline) override;
        void endRenderPass() override;
        void draw(const u32 vertices, const u32 instances, const u32 baseVertex,
                  const u32 baseInstance) override;
        void draw(const u32 indexCount, const u32 instances,
                  const u32 baseIndex, const i32 vertexOffset,
                  const u32 baseInstance) override;
        void bind(const IGraphicsPipeline* pipeline) override;
        void bind(const vector<IBuffer*>& buffers, const vector<u64>& offsets,
                  u32 first) override;
        void bind(const vector<IDescriptorSet*> descriptorSets,
                  const IGraphicsPipeline* pipeline, u32 first) override;
        void bind(IBuffer* elements, u64 offset) override;
        void copy(IBuffer* src, IBuffer* dst,
                  const vector<BufferCopyRegion>& regions) override;
        void copy(IBuffer* src, IImage* dst,
                  const vector<BufferImageCopyRegion>& regions,
                  const EImageLayout format) override;
        void barrier(EPipelineStageFlags src, EPipelineStageFlags dst,
                     EDependencyFlags dependency,
                     const vector<ImageMemoryBarrier>& imageBarriers) override;

        bool destroyed = false;
        VulkanCommandPool* pool = nullptr;
        VkCommandBuffer buffer = VK_NULL_HANDLE;

        HELIOS_NO_COPY_MOVE(VulkanCommandBuffer)
    };
} // namespace helios