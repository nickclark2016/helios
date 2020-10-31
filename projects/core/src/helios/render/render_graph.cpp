#include <helios/render/render_graph.hpp>

#include <helios/core/engine_context.hpp>

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

    std::string_view ImageResource::name() const
    {
        return std::string_view(_name);
    }

    ImageResourceInfo ImageResource::info() const noexcept
    {
        return _info;
    }

    RenderGraph::~RenderGraph()
    {
        for (auto& [name, resource] : _image)
        {
            delete resource;
        }
    }

    ImageResource& RenderGraph::addImageResource(std::string_view name, const ImageResourceInfo& info)
    {
        EngineContext::RenderContext& renderCtx = EngineContext::instance().render();

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
            IWindow& win = EngineContext::instance().window();
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

        _image[std::string(name)] = resource;
        return *resource;
    }
} // namespace helios