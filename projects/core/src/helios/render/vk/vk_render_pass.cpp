#include <helios/render/vk/vk_render_pass.hpp>

#include <helios/containers/vector.hpp>
#include <helios/render/bldr/render_pass_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_framebuffer.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    RenderPassBuilder::RenderPassBuilder()
    {
        _impl = new RenderPassBuilderImpl;
    }

    RenderPassBuilder::~RenderPassBuilder()
    {
        delete _impl;
    }

    RenderPassBuilder& RenderPassBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::attachments(const vector<AttachmentDescription>& attachments)
    {
        _impl->attachments = attachments;
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::subpasses(const vector<SubpassDescription>& subpasses)
    {
        _impl->subpasses = subpasses;
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::dependencies(const vector<SubpassDependency>& dependencies)
    {
        _impl->dependencies = dependencies;
        return *this;
    }

    IRenderPass* RenderPassBuilder::build() const
    {
        VulkanRenderPass* pass = new VulkanRenderPass;
        pass->device = cast<VulkanDevice*>(_impl->device);

        vector<VkAttachmentDescription> attachments;
        vector<VkSubpassDescription> subpasses;
        vector<VkSubpassDependency> dependencies;

        vector<vector<VkAttachmentReference>> inputAttachments;
        vector<vector<VkAttachmentReference>> colorAttachments;
        vector<vector<VkAttachmentReference>> resolveAttachments;
        vector<VkAttachmentReference> depthAttachments;
        vector<vector<u32>> preserveAttachments;

        attachments.reserve(_impl->attachments.size());
        subpasses.reserve(_impl->subpasses.size());
        dependencies.reserve(_impl->dependencies.size());
        inputAttachments.reserve(subpasses.size());
        colorAttachments.reserve(subpasses.size());
        resolveAttachments.reserve(subpasses.size());
        depthAttachments.reserve(subpasses.size());
        preserveAttachments.reserve(subpasses.size());

        for (const auto& sub : _impl->subpasses)
        {
            vector<VkAttachmentReference> inputs;
            vector<VkAttachmentReference> colors;
            vector<VkAttachmentReference> resolves;
            vector<u32> preserve;
            VkAttachmentReference depth;

            for (const auto& ref : sub.inputAttachments)
            {
                inputs.push_back({ref.attachment, static_cast<VkImageLayout>(ref.layout)});
            }

            for (const auto& ref : sub.colorAttachments)
            {
                colors.push_back({ref.attachment, static_cast<VkImageLayout>(ref.layout)});
            }

            for (const auto& ref : sub.resolveAttachments)
            {
                resolves.push_back({ref.attachment, static_cast<VkImageLayout>(ref.layout)});
            }

            if (sub.depthAttachment.has_value())
            {
                depth = {sub.depthAttachment.value().attachment,
                         static_cast<VkImageLayout>(sub.depthAttachment.value().layout)};
            }

            preserve = sub.preserveAttachments;

            inputAttachments.push_back(inputs);
            colorAttachments.push_back(colors);
            resolveAttachments.push_back(resolves);
            depthAttachments.push_back(depth);
            preserveAttachments.push_back(preserve);

            subpasses.push_back(
                {0, static_cast<VkPipelineBindPoint>(sub.bind), static_cast<u32>(inputs.size()),
                 inputAttachments[inputAttachments.size() - 1].data(), static_cast<u32>(colors.size()),
                 colorAttachments[colorAttachments.size() - 1].data(),
                 resolveAttachments[resolveAttachments.size() - 1].data(),
                 sub.depthAttachment.has_value() ? &depthAttachments[depthAttachments.size() - 1] : nullptr,
                 static_cast<u32>(preserve.size()), preserveAttachments[preserveAttachments.size() - 1].data()});
        }

        for (const auto& attachment : _impl->attachments)
        {
            attachments.push_back({0, static_cast<VkFormat>(attachment.format),
                                   static_cast<VkSampleCountFlagBits>(attachment.samples),
                                   static_cast<VkAttachmentLoadOp>(attachment.loadOp),
                                   static_cast<VkAttachmentStoreOp>(attachment.storeOp),
                                   static_cast<VkAttachmentLoadOp>(attachment.stencilLoadOp),
                                   static_cast<VkAttachmentStoreOp>(attachment.stencilStoreOp),
                                   static_cast<VkImageLayout>(attachment.initialLayout),
                                   static_cast<VkImageLayout>(attachment.finalLayout)});
        }

        for (const auto& dependency : _impl->dependencies)
        {
            dependencies.push_back({dependency.srcSubpass, dependency.dstSubpass, dependency.srcStageMask,
                                    dependency.dstStageMask, dependency.srcAccessMask, dependency.dstAccessMask,
                                    dependency.dependencyFlags});
        }

        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = static_cast<u32>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.dependencyCount = static_cast<u32>(dependencies.size());
        createInfo.pDependencies = dependencies.data();
        createInfo.subpassCount = static_cast<u32>(subpasses.size());
        createInfo.pSubpasses = subpasses.data();

        vkCreateRenderPass(pass->device->device, &createInfo, nullptr, &pass->renderpass);
        pass->device->renderPasses.push_back(pass);

        return pass;
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (const auto& fb : framebuffers)
            {
                delete fb;
            }

            if (!device->destroyed)
            {
                device->renderPasses.erase(std::find(device->renderPasses.begin(), device->renderPasses.end(), this));
            }

            vkDestroyRenderPass(device->device, renderpass, nullptr);
        }
    }
} // namespace helios