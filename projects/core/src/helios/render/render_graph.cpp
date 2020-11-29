#include <helios/render/render_graph.hpp>

#include <helios/containers/unordered_map.hpp>
#include <helios/containers/vector.hpp>
#include <helios/core/engine_context.hpp>

#define HELIOS_DEPTH_ATTACHMENT_INTERNAL_NAME "HELIOS_DEPTH_ATTACHMENT"

namespace helios
{
    ImageResource::~ImageResource()
    {
        for (auto image : _image) delete image;
        delete _sampler;
        for (auto view : _view) delete view;
    }

    IImage& ImageResource::image(u32 frame)
    {
        return *_image[frame];
    }

    IImageView& ImageResource::view(u32 frame)
    {
        return *_view[frame];
    }

    ISampler& ImageResource::sampler()
    {
        return *_sampler;
    }

    std::string ImageResource::name() const
    {
        return _name;
    }

    ImageResourceInfo ImageResource::info() const noexcept
    {
        return _info;
    }

    EFormat ImageResource::format() const noexcept
    {
        return _fmt;
    }

    ESampleCountFlagBits ImageResource::samples() const noexcept
    {
        return _samples;
    }

    BufferResource::~BufferResource()
    {
        for (auto buffer : _buffers) delete buffer;
    }

    IBuffer& BufferResource::buffer(const u32 frame)
    {
        return *_buffers[frame];
    }

    BufferResourceInfo BufferResource::info() const
    {
        return _info;
    }

    std::string BufferResource::name() const
    {
        return _name;
    }

    RenderPass::~RenderPass()
    {
        for (auto target : _renderTargets)
        {
            delete target;
        }

        for (auto& [name, shader] : _shaders)
        {
            delete shader;
        }

        delete _pass;
    }

    Result<RenderPass> RenderPass::addColorAttachment(const std::string& name, const ImageAccessInfo& access)
    {
        _colorAttachmentInfos[name] = access;
        ImageResource* resource = _graph->_getImageByName(name);
        _colorAttachments[name] = resource;

        return Result<RenderPass>::fromConditional(resource != nullptr, this,
                                                   static_cast<u64>(ERenderGraphError::RESOURCE_NOT_FOUND));
    }

    Result<RenderPass> RenderPass::addDepthAttachment(const std::string& name, const ImageAccessInfo& access)
    {
        _depthAttachmentInfo = access;
        _depthAttachment = _graph->_getImageByName(name);

        return Result<RenderPass>::fromConditional(_depthAttachment != nullptr, this,
                                                   static_cast<u64>(ERenderGraphError::RESOURCE_NOT_FOUND));
    }

    Result<RenderPass> RenderPass::addInputAttachment(const std::string& name, const ImageAccessInfo& access)
    {
        _inputAttachmentInfos[name] = access;
        ImageResource* resource = _graph->_getImageByName(name);
        _inputAttachments[name] = resource;

        return Result<RenderPass>::fromConditional(resource != nullptr, this,
                                                   static_cast<u64>(ERenderGraphError::RESOURCE_NOT_FOUND));
    }

    Result<RenderPass> RenderPass::addUniformBuffer(const std::string& name, const BufferAccessInfo& access)
    {
        _uniformBufferInfos[name] = access;
        BufferResource* resource = _graph->_getBufferByName(name);
        _uniformBuffers[name] = resource;

        return Result<RenderPass>::fromConditional(resource != nullptr, this,
                                                   static_cast<u64>(ERenderGraphError::RESOURCE_NOT_FOUND));
    }

    Result<RenderPass> RenderPass::succeeds(const std::string& pass)
    {
        _succeeds.push_back(pass);
        return Result<RenderPass>::fromSuccess(this);
    }

    Result<RenderPass> RenderPass::preceeds(const std::string& pass)
    {
        _preceeds.push_back(pass);
        return Result<RenderPass>::fromSuccess(this);
    }

    Result<RenderPass> RenderPass::attachShader(const std::string& name, const std::string& vertex,
                                                const std::string& fragment)
    {
        _shaderInfos[name] = {vertex, fragment};
        return Result<RenderPass>::fromSuccess(this);
    }

    std::string RenderPass::name() const
    {
        return _name;
    }

    bool RenderPass::_build()
    {
        EngineContextFactory factory;
        EngineContext& ctx = factory.create();

        RenderPassBuilder::SubpassDescription description;
        description.bind = EBindPoint::GRAPHICS;
        
        unordered_map<std::string, u32> imageIndices;

        size_t attachmentCount = _inputAttachmentInfos.size() + _colorAttachmentInfos.size() + _depthAttachment ? 1 : 0;

        if (attachmentCount == 0)
        {
            return false;
        }

        vector<RenderPassBuilder::AttachmentDescription> attachmentDescs;
        vector<ImageResource*> images;
        attachmentDescs.reserve(attachmentCount);

        for (auto& [name, info] : _inputAttachmentInfos)
        {
            ImageResource* image = _inputAttachments[name];

            images.push_back(image);
            const u32 imageIndex = static_cast<u32>(imageIndices.size());
            imageIndices[name] = imageIndex;

            RenderPassBuilder::AttachmentDescription desc;
            desc.initialLayout = desc.finalLayout = info.layout;
            desc.format = image->format();
            desc.loadOp = info.loadOp;
            desc.storeOp = info.storeOp;
            desc.stencilLoadOp = info.stencilLoadOp;
            desc.stencilStoreOp = info.stencilStoreOp;
            attachmentDescs.push_back(desc);
        }

        for (auto& [name, info] : _colorAttachmentInfos)
        {
            ImageResource* image = _colorAttachments[name];

            images.push_back(image);
            const u32 imageIndex = static_cast<u32>(imageIndices.size());
            imageIndices[name] = imageIndex;

            RenderPassBuilder::AttachmentDescription desc;
            desc.initialLayout = desc.finalLayout = info.layout;
            desc.format = image->format();
            desc.loadOp = info.loadOp;
            desc.storeOp = info.storeOp;
            desc.stencilLoadOp = info.stencilLoadOp;
            desc.stencilStoreOp = info.stencilStoreOp;
            attachmentDescs.push_back(desc);
        }

        if (_depthAttachment)
        {
            ImageResource* image = _depthAttachment;

            images.push_back(image);
            const u32 imageIndex = static_cast<u32>(imageIndices.size());
            imageIndices[HELIOS_DEPTH_ATTACHMENT_INTERNAL_NAME] = imageIndex;

            RenderPassBuilder::AttachmentDescription desc;
            desc.initialLayout = desc.finalLayout = _depthAttachmentInfo.layout;
            desc.format = image->format();
            desc.loadOp = _depthAttachmentInfo.loadOp;
            desc.storeOp = _depthAttachmentInfo.storeOp;
            desc.stencilLoadOp = _depthAttachmentInfo.stencilLoadOp;
            desc.stencilStoreOp = _depthAttachmentInfo.stencilStoreOp;
            attachmentDescs.push_back(desc);
        }

        RenderPassBuilder::SubpassDescription subpass;
        subpass.bind = EBindPoint::GRAPHICS;

        for (auto& [name, info] : _colorAttachmentInfos)
        {
            RenderPassBuilder::AttachmentReference ref;
            ref.attachment = imageIndices[name];
            ref.layout = info.layout;
            subpass.colorAttachments.push_back(ref);
        }

        for (auto& [name, info] : _inputAttachmentInfos)
        {
            RenderPassBuilder::AttachmentReference ref;
            ref.attachment = imageIndices[name];
            ref.layout = info.layout;
            subpass.inputAttachments.push_back(ref);
        }

        if (_depthAttachment)
        {
            RenderPassBuilder::AttachmentReference ref;
            ref.attachment = imageIndices[HELIOS_DEPTH_ATTACHMENT_INTERNAL_NAME];
            ref.layout = _depthAttachmentInfo.layout;
            subpass.depthAttachment = ref;
        }

        // TODO: Subpass Dependencies for External Subpasses
        _pass = RenderPassBuilder().device(&ctx.render().device()).attachments(attachmentDescs).subpasses({subpass}).build();

        const u32 frameCount = ctx.render().swapchain().imagesCount();
        _renderTargets.reserve(frameCount);

        for (u32 i = 0; i < frameCount; ++i)
        {
            vector<IImageView*> views;
            views.reserve(images.size());

            u32 minWidth = ~0UL;
            u32 minHeight = ~0UL;
            u32 minLayers = ~0UL;

            for (const auto& imageView : images)
            {
                const bool isPerFrameResource = imageView->info().perFrameInFlightResource;
                IImageView& view = isPerFrameResource ? imageView->view() : imageView->view(i);
                views.push_back(&view);

                IImage* img = view.image();
                minWidth = min(minWidth, img->width());
                minHeight = min(minHeight, img->height());
                minLayers = min(minLayers, img->layers());
            }

            IFramebuffer* renderTarget = FramebufferBuilder().attachments(views)
                .renderpass(_pass)
                .width(minWidth)
                .height(minHeight)
                .layers(minLayers)
                .build();

            _renderTargets.push_back(renderTarget);
        }

        return true;
    }

    RenderGraph::~RenderGraph()
    {
        for (auto& [name, resource] : _images)
        {
            delete resource;
        }

        for (auto& [name, resource] : _uniforms)
        {
            delete resource;
        }

        for (auto& [name, pass] : _renderPasses)
        {
            delete pass;
        }
    }

    Result<ImageResource> RenderGraph::addImageResource(const std::string& name, const ImageResourceInfo& info)
    {
        EngineContextFactory engineCtxFactory;
        EngineContext& engineCtx = engineCtxFactory.create();
        EngineContext::RenderContext& renderCtx = engineCtx.render();

        const auto resourceCount = info.perFrameInFlightResource ? renderCtx.swapchain().imagesCount() : 1;

        ImageResource* resource = new ImageResource;
        resource->_info = info;
        resource->_name = name;

        ImageBuilder imgBldr;
        imgBldr.device(&renderCtx.device())
            .type(info.type)
            .format(info.format)
            .mipLevels(info.mips)
            .arrayLayers(info.layers)
            .samples(info.samples)
            .tiling(info.tiling)
            .usage(info.usage)
            .initialLayout(info.layout)
            .preferredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
            .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
            .memoryUsage(EMemoryUsage::GPU_ONLY);

        switch (info.dimensions.dimension)
        {
        case EDimensionType::ABSOLUTE:
            imgBldr.extent(info.dimensions.absolute.width, info.dimensions.absolute.height,
                           info.dimensions.absolute.depth);
            break;
        case EDimensionType::RELATIVE:
            IWindow& win = EngineContextFactory().create().window();
            const u32 width = win.width() * info.dimensions.relative.width;
            const u32 height = win.height() * info.dimensions.relative.height;
            const u32 depth = 1U;
            imgBldr.extent(width, height, depth);
            break;
        }

        for (auto i = 0U; i < resourceCount; ++i)
        {
            resource->_image.push_back(imgBldr.build());
        }

        for (auto i = 0U; i < resourceCount; ++i)
        {
            resource->_view.push_back(ImageViewBuilder()
                                          .image(resource->_image[i])
                                          .baseMipLevel(0)
                                          .mipLevels(info.mips)
                                          .baseArrayLayer(0)
                                          .arrayLayers(info.layers)
                                          .format(info.format)
                                          .type(info.viewType)
                                          .aspect(info.aspect)
                                          .build());
        }

        if (info.sampler.has_value())
        {
            SamplerBuilder samplerBldr;
            samplerBldr.device(&renderCtx.device())
                .magnification(info.sampler->magnification)
                .minification(info.sampler->minification)
                .mipmap(info.sampler->mipMode)
                .addressModeU(info.sampler->addressModeU)
                .addressModeV(info.sampler->addressModeV)
                .addressModeW(info.sampler->addressModeW)
                .mipLodBias(info.sampler->mipLevelOfDetailBias);

            if (info.sampler->anisotropy.has_value())
            {
                samplerBldr.anisotropy(*info.sampler->anisotropy);            
            }

            if (info.sampler->compareOp.has_value())
            {
                samplerBldr.compare(*info.sampler->compareOp);
            }

            resource->_sampler = samplerBldr.minLod(info.sampler->minLevelOfDetail)
                .maxLod(info.sampler->maxLevelOfDetail)
                .border(info.sampler->borderColor)
                .unnormalized(info.sampler->unnormalizedCoordinates)
                .build();
        }
        else
        {
            resource->_sampler = nullptr;
        }

        resource->_fmt = info.format;
        resource->_samples = info.samples;

        _images[name] = resource;
        return Result<ImageResource>::fromSuccess(resource);
    }

    Result<BufferResource> RenderGraph::addUniformBufferResource(const std::string& name, const BufferResourceInfo& info)
    {
        BufferResource* uniformBuffer = new BufferResource();
        uniformBuffer->_info = info;
        uniformBuffer->_name = name;

        EngineContext::RenderContext& renderCtx = EngineContextFactory().create().render();
        const auto resourceCount = info.perFrameInFlightResource ? renderCtx.swapchain().imagesCount() : 1;
        uniformBuffer->_buffers.reserve(resourceCount);

        EMemoryUsage memoryUsage = info.deviceLocal ? EMemoryUsage::GPU_ONLY : EMemoryUsage::CPU_TO_GPU;

        EMemoryPropertyFlags memoryFlags = 0;
        memoryFlags |= info.deviceLocal ? MEMORY_PROPERTY_DEVICE_LOCAL : 0;
        memoryFlags |= info.hostVisible ? MEMORY_PROPERTY_HOST_VISIBLE : 0;
        memoryFlags |= info.hostCached ? MEMORY_PROPERTY_HOST_CACHED : 0;
        memoryFlags |= info.hostCoherent ? MEMORY_PROPERTY_HOST_COHERENT : 0;

        EBufferTypeFlags bufferType = BUFFER_TYPE_UNIFORM;
        bufferType |= info.transferSrc ? BUFFER_TYPE_TRANSFER_SRC : 0;
        bufferType |= info.transferDst ? BUFFER_TYPE_TRANSFER_DST : 0;

        BufferBuilder bufferBldr;
        bufferBldr.device(&renderCtx.device())
            .size(info.size)
            .memoryUsage(memoryUsage)
            .requiredFlags(memoryFlags)
            .preferredFlags(memoryFlags)
            .usage(bufferType);

        for (u64 i = 0; i < resourceCount; ++i)
        {
            auto buf = bufferBldr.build();
            uniformBuffer->_buffers.push_back(buf);
        }

        _uniforms[name] = uniformBuffer;
        return Result<BufferResource>::fromSuccess(uniformBuffer);
    }

    Result<RenderPass> RenderGraph::createPass(const std::string& name)
    {
        RenderPass* pass = new RenderPass();
        pass->_name = name;
        pass->_graph = this;

        _renderPasses[name] = pass;
        return Result<RenderPass>::fromSuccess(pass);
    }

    bool RenderGraph::build()
    {
        // Build all render passes
        for (const auto& [name, pass] : _renderPasses)
        {
            const bool result = pass->_build();
            if (result == false)
            {
                return false;
            }
        }
        
        // TODO: Build all barriers

        return true;
    }

    ImageResource* RenderGraph::_getImageByName(const std::string& name)
    {
        const auto it = _images.find(name);
        if (it == _images.end())
        {
            return nullptr;
        }
        return it->second;
    }

    BufferResource* RenderGraph::_getBufferByName(const std::string& name)
    {
        const auto it = _uniforms.find(name);
        if (it == _uniforms.end())
        {
            return nullptr;
        }
        return it->second;
    }
} // namespace helios