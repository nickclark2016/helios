#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

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
        void bind(const IGraphicsPipeline* pipeline) override;
        void draw(const uint32_t vertices, const uint32_t instances,
                  const uint32_t baseVertex,
                  const uint32_t baseInstance) override;

        bool destroyed = false;
        VulkanCommandPool* pool = nullptr;
        VkCommandBuffer buffer = VK_NULL_HANDLE;

        HELIOS_NO_COPY_MOVE(VulkanCommandBuffer)
    };
} // namespace helios