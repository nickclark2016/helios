#include <helios/render/vk/vk_framebuffer.hpp>

#include <helios/render/bldr/framebuffer_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>
#include <helios/render/vk/vk_image_view.hpp>
#include <helios/render/vk/vk_render_pass.hpp>
#include <helios/utility.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    FramebufferBuilder::FramebufferBuilder()
    {
        _impl = new FramebufferBuilderImpl;
    }

    FramebufferBuilder::~FramebufferBuilder()
    {
        delete _impl;
    }

    FramebufferBuilder& FramebufferBuilder::renderpass(
        const IRenderPass* renderpass)
    {
        _impl->renderpass = const_cast<IRenderPass*>(renderpass);
        return *this;
    }

    FramebufferBuilder& FramebufferBuilder::attachments(
        const vector<IImageView*>& attachments)
    {
        _impl->views = attachments;
        return *this;
    }

    FramebufferBuilder& FramebufferBuilder::width(const uint32_t width)
    {
        _impl->width = width;
        return *this;
    }

    FramebufferBuilder& FramebufferBuilder::height(const uint32_t height)
    {
        _impl->height = height;
        return *this;
    }

    FramebufferBuilder& FramebufferBuilder::layers(const uint32_t layers)
    {
        _impl->layers = layers;
        return *this;
    }

    IFramebuffer* FramebufferBuilder::build() const
    {
        VulkanFramebuffer* framebuffer = new VulkanFramebuffer;
        framebuffer->pass = cast<VulkanRenderPass*>(_impl->renderpass);
        framebuffer->fbWidth = _impl->width;
        framebuffer->fbHeight = _impl->height;
        framebuffer->fbLayers = _impl->layers;

        vector<VkImageView> views;

        for (const auto& view : _impl->views)
        {
            auto vkView = cast<VulkanImageView*>(view);
            views.push_back(vkView->view);
            framebuffer->images.emplace_back(vkView);
        }

        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.attachmentCount = static_cast<uint32_t>(views.size());
        info.pAttachments = views.data();
        info.width = _impl->width;
        info.height = _impl->height;
        info.layers = _impl->layers;
        info.renderPass =
            cast<VulkanRenderPass*>(_impl->renderpass)->renderpass;

        vkCreateFramebuffer(framebuffer->pass->device->device, &info, nullptr,
                            &framebuffer->fb);
        framebuffer->pass->framebuffers.push_back(framebuffer);

        return framebuffer;
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        if (!destroyed)
        {
            if (!pass->destroyed)
            {
                pass->framebuffers.erase(std::find(pass->framebuffers.begin(),
                                                   pass->framebuffers.end(),
                                                   this));
            }

            vkDestroyFramebuffer(pass->device->device, fb, nullptr);
        }
    }

    uint32_t VulkanFramebuffer::width() const
    {
        return fbWidth;
    }

    uint32_t VulkanFramebuffer::height() const
    {
        return fbHeight;
    }

    uint32_t VulkanFramebuffer::layers() const
    {
        return fbLayers;
    }

    vector<IImageView*> VulkanFramebuffer::attachments() const
    {
        vector<IImageView*> imgs;
        for (const auto& image : images)
        {
            imgs.push_back(image);
        }
        return imgs;
    }
} // namespace helios