#include <helios/render/render_graph.hpp>

#include <helios/render/graphics.hpp>

#include <unordered_set>

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

    bool RenderPass::setSampleCount(const std::string& name,
                                    const ESampleCountFlagBits samples)
    {
        auto it = _parent._viewResources.find(name);

        if (_attachments.find(it->second) == _attachments.end())
        {
            return false;
        }

        _attachments[it->second].samples = samples;

        return true;
    }

    bool RenderPass::setLoadAction(const std::string& name,
                                   const EAttachmentLoadOp op)
    {
        auto it = _parent._viewResources.find(name);
        if (it == _parent._viewResources.end())
        {
            return false;
        }

        if (_attachments.find(it->second) == _attachments.end())
        {
            return false;
        }

        _attachments[it->second].loadOp = op;

        return true;
    }

    bool RenderPass::setStoreAction(const std::string& name,
                                    const EAttachmentStoreOp op)
    {
        auto it = _parent._viewResources.find(name);

        if (_attachments.find(it->second) == _attachments.end())
        {
            return false;
        }

        _attachments[it->second].storeOp = op;

        return true;
    }

    bool RenderPass::setStencilLoadAction(const std::string& name,
                                          const EAttachmentLoadOp op)
    {
        auto it = _parent._viewResources.find(name);

        if (_attachments.find(it->second) == _attachments.end())
        {
            return false;
        }

        _attachments[it->second].stencilLoadOp = op;

        return true;
    }

    bool RenderPass::setStencilStoreAction(const std::string& name,
                                           const EAttachmentStoreOp op)
    {
        auto it = _parent._viewResources.find(name);

        if (_attachments.find(it->second) == _attachments.end())
        {
            return false;
        }

        _attachments[it->second].stencilStoreOp = op;

        return true;
    }

    bool RenderPass::setLayoutTransition(const std::string& name,
                                         const EImageLayout initLayout,
                                         const EImageLayout finalLayout)
    {
        auto it = _parent._viewResources.find(name);

        if (_attachments.find(it->second) == _attachments.end())
        {
            return false;
        }

        _attachments[it->second].initialLayout = initLayout;
        _attachments[it->second].finalLayout = finalLayout;

        return true;
    }

    void RenderPass::_build()
    {
        vector<RenderPassBuilder::AttachmentDescription> attachments;
        vector<RenderPassBuilder::SubpassDescription> subpasses;
        vector<RenderPassBuilder::SubpassDependency> internalDeps;

        std::unordered_map<u32, u32> imageIdToIndex;
        std::unordered_map<u32, u32> imageToLastSubpass;

        // Sort subpasses
        vector<vector<SubPass*>> adjacencies;
        adjacencies.resize(_passes.size());
        for (const auto& p : _passes)
        {
            for (const auto& dep : p->_dependsOn)
            {
                auto& node = _passes[dep];
                auto nodeId = node->_id;
                adjacencies[nodeId].push_back(p);
            }
        }

        linked_list<SubPass*> sorted;
        vector<bool> visited(_passes.size(), false);
        for (auto& visit : visited)
            visit = false;

        for (auto& pass : _passes)
        {
            if (!visited[pass->_id])
            {
                _topoSortRecursive(pass, adjacencies, visited, sorted);
            }
        }

        std::unordered_map<u32, u32> subpassIdToIndex;
        u32 subpassIndex = 0;
        for (const auto& pass : sorted)
        {
            subpassIdToIndex[pass->_id] = subpassIndex;
            for (const auto img : pass->_colorOutputs)
            {
                auto it = imageIdToIndex.find(img);
                if (it == imageIdToIndex.end())
                {
                    attachments.push_back(_attachments[img]);
                    imageIdToIndex[img] =
                        static_cast<u32>(imageIdToIndex.size());
                }

                imageToLastSubpass[img] = subpassIndex;
            }

            for (const auto img : pass->_inputAttachments)
            {
                auto it = imageIdToIndex.find(img);
                if (it == imageIdToIndex.end())
                {
                    attachments.push_back(_attachments[img]);
                    imageIdToIndex[img] =
                        static_cast<u32>(imageIdToIndex.size());
                }

                imageToLastSubpass[img] = subpassIndex;
            }

            if (pass->_depthAttachment >= 0)
            {
                auto img = pass->_depthAttachment;
                auto it = imageIdToIndex.find(img);
                if (it == imageIdToIndex.end())
                {
                    attachments.push_back(_attachments[img]);
                    imageIdToIndex[img] =
                        static_cast<u32>(imageIdToIndex.size());
                }

                imageToLastSubpass[img] = subpassIndex;
            }
        }

        vector<u32> attachmentsFound;
        attachmentsFound.reserve(imageIdToIndex.size());

        subpassIndex = 0;
        for (auto& pass : sorted)
        {
            RenderPassBuilder::SubpassDescription desc;
            desc.bind = EBindPoint::GRAPHICS;

            for (auto color : pass->_colorOutputs)
            {
                auto id = imageIdToIndex[color];
                desc.colorAttachments.push_back(
                    {id, EImageLayout::COLOR_ATTACHMENT_OPTIMAL});
                attachmentsFound.push_back(id);
            }

            for (auto input : pass->_inputAttachments)
            {
                auto id = imageIdToIndex[input];
                desc.inputAttachments.push_back(
                    {id, EImageLayout::SHADER_READ_ONLY_OPTIMAL});
                attachmentsFound.push_back(id);
            }

            if (pass->_depthAttachment >= 0)
            {
                auto id = imageIdToIndex[pass->_depthAttachment];
                desc.depthAttachment = {
                    id, EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
                attachmentsFound.push_back(id);
            }

            for (u32 i = 0; i < imageIdToIndex.size(); i++)
            {
                if (std::find(attachmentsFound.begin(), attachmentsFound.end(),
                              i) == attachmentsFound.end() &&
                    imageToLastSubpass[i] < subpassIndex)
                {
                    desc.preserveAttachments.push_back(i);
                }
            }

            attachmentsFound.clear();

            subpasses.push_back(desc);
            ++subpassIndex;
        }

        subpassIndex = 0;
        for (auto pass : sorted)
        {
            for (u32 i = 0; i < pass->_dependsOn.size(); i++)
            {
                RenderPassBuilder::SubpassDependency spDep;

                spDep.srcSubpass = i;
                spDep.dstSubpass =
                    subpassIdToIndex[_passes[pass->_dependsOn[i]]->_id];
                spDep.srcStageMask = pass->_dependencyInfos[i].srcStages;
                spDep.dstStageMask = pass->_dependencyInfos[i].dstStages;
                spDep.srcAccessMask = pass->_dependencyInfos[i].srcAccess;
                spDep.dstAccessMask = pass->_dependencyInfos[i].dstAccess;
                spDep.dependencyFlags =
                    pass->_dependencyInfos[i].dependencyFlags;

                internalDeps.push_back(spDep);
            }

            ++subpassIndex;
        }

        _pass = RenderPassBuilder()
                    .device(_parent._device)
                    .attachments(attachments)
                    .subpasses(subpasses)
                    .dependencies(internalDeps)
                    .build();
    }

    void RenderPass::_topoSortRecursive(SubPass* root,
                                        vector<vector<SubPass*>>& nodes,
                                        vector<bool>& visited,
                                        linked_list<SubPass*>& stack)
    {
        visited[root->_id] = true;
        for (SubPass* pass : nodes[root->_id])
        {
            if (!visited[pass->_id])
            {
                _topoSortRecursive(pass, nodes, visited, stack);
            }
        }
        stack.push_front(root);
    }

    SubPass::SubPass(RenderPass& parent, const std::string& name, const i32 id)
        : _parent(parent), _id(id), _name(name), _depthAttachment(-1)
    {
    }

    bool SubPass::addColorOutputAttachment(const std::string& name)
    {
        auto it = _parent._parent._viewResources.find(name);
        if (it == _parent._parent._viewResources.end())
        {
            return false;
        }

        if (_parent._attachments.find(it->second) == _parent._attachments.end())
        {
            RenderPassBuilder::AttachmentDescription desc;
            desc.format =
                _parent._parent
                    ._images[_parent._parent._imageResources
                                 [_parent._parent._viewInfos[it->second].image]]
                    ->format();
            desc.samples = SAMPLE_COUNT_1;
            desc.loadOp = EAttachmentLoadOp::DONT_CARE;
            desc.storeOp = EAttachmentStoreOp::DONT_CARE;
            desc.stencilLoadOp = EAttachmentLoadOp::DONT_CARE;
            desc.stencilStoreOp = EAttachmentStoreOp::DONT_CARE;
            desc.initialLayout = EImageLayout::UNDEFINED;
            desc.finalLayout = EImageLayout::UNDEFINED;

            _parent._attachments[it->second] = desc;
        }

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
        auto it = _parent._parent._viewResources.find(name);
        if (it == _parent._parent._viewResources.end())
        {
            return false;
        }

        if (_parent._attachments.find(it->second) == _parent._attachments.end())
        {
            RenderPassBuilder::AttachmentDescription desc;
            desc.format =
                _parent._parent
                    ._images[_parent._parent._imageResources
                                 [_parent._parent._viewInfos[it->second].image]]
                    ->format();
            desc.samples = SAMPLE_COUNT_1;
            desc.loadOp = EAttachmentLoadOp::DONT_CARE;
            desc.storeOp = EAttachmentStoreOp::DONT_CARE;
            desc.stencilLoadOp = EAttachmentLoadOp::DONT_CARE;
            desc.stencilStoreOp = EAttachmentStoreOp::DONT_CARE;
            desc.initialLayout = EImageLayout::UNDEFINED;
            desc.finalLayout = EImageLayout::UNDEFINED;

            _parent._attachments[it->second] = desc;
        }

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
        auto it = _parent._parent._viewResources.find(name);
        if (it == _parent._parent._viewResources.end())
        {
            return false;
        }

        if (_parent._attachments.find(it->second) == _parent._attachments.end())
        {
            RenderPassBuilder::AttachmentDescription desc;
            desc.format =
                _parent._parent
                    ._images[_parent._parent._imageResources
                                 [_parent._parent._viewInfos[it->second].image]]
                    ->format();
            desc.samples = SAMPLE_COUNT_1;
            desc.loadOp = EAttachmentLoadOp::DONT_CARE;
            desc.storeOp = EAttachmentStoreOp::DONT_CARE;
            desc.stencilLoadOp = EAttachmentLoadOp::DONT_CARE;
            desc.stencilStoreOp = EAttachmentStoreOp::DONT_CARE;
            desc.initialLayout = EImageLayout::UNDEFINED;
            desc.finalLayout = EImageLayout::UNDEFINED;

            _parent._attachments[it->second] = desc;
        }

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

    bool SubPass::dependsOn(const std::string& pass,
                            const PassDependencyInfo& dependencyInfo)
    {
        SubPass* dep = _parent.getSubpass(pass);
        if (dep == nullptr)
        {
            return false;
        }

        _dependsOn.push_back(dep->_id);
        _dependencyInfos.push_back(dependencyInfo);
        return true;
    }

    RenderGraph::RenderGraph(IDevice* device, ISwapchain* swapchain)
        : _device(device), _swapchain(swapchain)
    {
    }

    RenderGraph::~RenderGraph()
    {
        _views.clear();
        _images.clear();
        _passes.clear();
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
            .initialLayout(EImageLayout::UNDEFINED)
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
        if (imageIt == _imageResources.end() || viewIt != _viewResources.end())
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

    void RenderGraph::releaseResources()
    {
        for (auto& view : _views)
        {
            delete view;
        }

        for (auto& img : _images)
        {
            delete img;
        }

        for (auto& pass : _passes)
        {
            delete pass;
        }

        _passes.clear();
        _views.clear();
        _images.clear();
    }

} // namespace helios
