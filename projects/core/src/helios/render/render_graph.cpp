#include <helios/render/render_graph.hpp>

#include <helios/render/graphics.hpp>

namespace helios
{
    RenderPass::RenderPass(RenderGraph& parent, const std::string& name, u32 id)
        : _parent(parent), _id(id), _pass(nullptr), _name(name)
    {
    }

    RenderPass::RenderPass(RenderPass&& other) noexcept
        : _parent(other._parent), _id(helios::move(other._id)),
          _pass(helios::move(other._pass)), _name(helios::move(other._name)),
          _passes(helios::move(other._passes))
    {
        other._passes.clear();
        _pass = nullptr;
    }

    RenderPass::~RenderPass()
    {
        for (auto& pass : _passes)
        {
            delete pass;
        }
        _passes.clear();

        delete _pass;
    }

    RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
    {
        // cleanup the contents
        for (auto& pass : _passes)
        {
            delete pass;
        }
        _passes.clear();

        delete _pass;

        // move the other pass
        _id = helios::move(other._id);
        _pass = helios::move(other._pass);
        _name = helios::move(other._name);
        _passes = helios::move(other._passes);

        // clear the other pass
        other._passes.clear();
        other._pass = nullptr;

        return *this;
    }

    SubPass& RenderPass::addSubpass(const std::string& name)
    {
        SubPass* pass =
            new SubPass(*this, name, static_cast<i32>(_passes.size()));
        _passes.push_back(pass);
        return *pass;
    }

    SubPass* RenderPass::getSubpass(const std::string& name)
    {
        for (auto& pass : _passes)
        {
            if (pass->_name == name)
            {
                return pass;
            }
        }

        return nullptr;
    }

    SubPass::SubPass(RenderPass& parent, const std::string& name, const i32 id)
        : _parent(parent), _id(id), _name(name), _depthAttachment(-1)
    {
    }

    bool SubPass::addColorOutputAttachment(const std::string& name)
    {
        // search for this in the render graph
        RenderGraph& graph = _parent._parent;
        auto viewIt = graph._viewResources.find(name);
        if (viewIt == graph._viewResources.end())
        {
            return false;
        }
        _colorOutputs.push_back(viewIt->second);

        return true;
    }

    bool SubPass::addInputAttachment(const std::string& name)
    {
        // search for this in the render graph
        RenderGraph& graph = _parent._parent;
        auto viewIt = graph._viewResources.find(name);
        if (viewIt == graph._viewResources.end())
        {
            return false;
        }
        _inputAttachments.push_back(viewIt->second);

        return true;
    }

    bool SubPass::addDepthStencilAttachment(const std::string& name)
    {
        // search for this in the render graph
        RenderGraph& graph = _parent._parent;
        auto viewIt = graph._viewResources.find(name);
        if (viewIt == graph._viewResources.end())
        {
            return false;
        }
        _depthAttachment = viewIt->second;

        return true;
    }

    bool SubPass::dependsOn(const std::string& pass)
    {
        SubPass* dep = _parent.getSubpass(pass);
        if (dep == nullptr)
        {
            return false;
        }

        _dependsOn.push_back(dep->_id);
        return true;
    }

    RenderGraph::RenderGraph(IDevice* device, ISwapchain* swapchain)
        : _device(device), _swapchain(swapchain)
    {
    }

    RenderGraph::~RenderGraph()
    {
        for (auto view : _views)
        {
            delete view;
        }
        _views.clear();

        for (auto image : _images)
        {
            delete image;
        }
        _images.clear();
    }

    bool RenderGraph::addImage(const ImageInfo& info)
    {
        // don't override the name
        if (_imageResources.find(info.name) != _imageResources.end())
        {
            return false;
        }

        u32 uuid = static_cast<u32>(_images.size());
        ImageBuilder imgBuilder;
        imgBuilder.device(_device)
            .type(info.type)
            .format(info.format)
            .mipLevels(info.mipLevels)
            .arrayLayers(info.arrayLayers)
            .samples(info.samples)
            .tiling(info.tiling)
            .usage(info.usage)
            .queues(info.queues)
            .initialLayout(info.layout)
            .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
            .memoryUsage(EMemoryUsage::GPU_ONLY);

        switch (info.sizeType)
        {
        case ESizeType::ABSOLUTE:
            imgBuilder.extent(static_cast<u32>(info.dimensions.width),
                              static_cast<u32>(info.dimensions.height),
                              static_cast<u32>(info.dimensions.depth));
            break;
        case ESizeType::SWAPCHAIN_RELATIVE:
            imgBuilder.extent(
                static_cast<u32>(static_cast<f32>(info.dimensions.height) *
                                 _swapchain->width()),
                static_cast<u32>(static_cast<f32>(info.dimensions.height) *
                                 _swapchain->height()),
                static_cast<u32>(static_cast<f32>(info.dimensions.depth) * 1U));
            break;
        default:;
        };

        auto img = imgBuilder.build();

        _images.push_back(img);
        _imageResources[info.name] = uuid;
        _imageInfos[uuid] = info;

        return true;
    }

    bool RenderGraph::addAttachment(const std::string& name,
                                    const TextureAttachmentResource& attachment)
    {
        const auto imageIt = _imageResources.find(attachment.image);
        const auto viewIt = _viewResources.find(name);
        if (imageIt == _imageResources.end() || viewIt == _viewResources.end())
        {
            return false;
        }

        u32 uuid = static_cast<u32>(_views.size());

        auto img = _images[imageIt->second];

        ImageViewBuilder viewBuilder;
        auto view = viewBuilder.image(img)
                        .aspect(attachment.aspect)
                        .baseArrayLayer(attachment.baseLayer)
                        .arrayLayers(attachment.layerCount)
                        .baseMipLevel(attachment.baseMip)
                        .mipLevels(attachment.mipCount)
                        .type(attachment.type)
                        .format(img->format())
                        .build();

        _views.push_back(view);
        _viewResources[name] = uuid;
        _viewInfos[uuid] = attachment;

        return true;
    }

    RenderPass& RenderGraph::addGraphicsPass(const std::string& name)
    {
        auto pass =
            new RenderPass(*this, name, static_cast<i32>(_passes.size()));
        _passes.push_back(pass);
        return *pass;
    }

    void RenderGraph::build()
    {
        // construct each render pass
        for (auto& pass : _passes)
        {
            pass->_build();
        }

        // perform internal pass ordering
    }

} // namespace helios
