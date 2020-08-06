#pragma once

#include <helios/containers/linked_list.hpp>
#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

#include <string>
#include <unordered_map>

namespace helios
{
    enum class ESizeType : u32
    {
        SWAPCHAIN_RELATIVE,
        ABSOLUTE
    };

    struct ImageDimensions
    {
        float width;
        float height;
        float depth;
    };

    struct ImageInfo
    {
        std::string name;
        ESizeType sizeType;
        ImageDimensions dimensions;
        EImageType type;
        EFormat format;
        u32 mipLevels;
        u32 arrayLayers;
        ESampleCountFlagBits samples;
        EImageTiling tiling;
        EImageUsageFlags usage;
        vector<IQueue*> queues;
    };

    struct TextureAttachmentResource
    {
        std::string image;
        EImageAspectFlags aspect;
        EImageViewType type;
        u32 baseLayer;
        u32 layerCount;
        u32 baseMip;
        u32 mipCount;
    };

    struct PassDependencyInfo
    {
        EShaderStageFlags srcStages;
        EShaderStageFlags dstStages;
        EAccessFlags srcAccess;
        EAccessFlags dstAccess;
        EDependencyFlags dependencyFlags;
    };

    class RenderGraph;
    class RenderPass;
    class SubPass;

    class RenderPass
    {
        friend class RenderGraph;
        friend class SubPass;

        RenderPass(RenderGraph& graph, const std::string& name, u32 id);

    public:
        HELIOS_NO_COPY(RenderPass);

        RenderPass(RenderPass&& other) noexcept;

        ~RenderPass();

        RenderPass& operator=(RenderPass&& other) noexcept;

        SubPass& addSubpass(const std::string& name);
        SubPass* getSubpass(const std::string& name);

        bool setSampleCount(const std::string& name,
                            const ESampleCountFlagBits samples);
        bool setLoadAction(const std::string& name, const EAttachmentLoadOp op);
        bool setStoreAction(const std::string& name,
                            const EAttachmentStoreOp op);
        bool setStencilLoadAction(const std::string& name,
                                  const EAttachmentLoadOp op);
        bool setStencilStoreAction(const std::string& name,
                                   const EAttachmentStoreOp op);
        bool setLayoutTransition(const std::string& name,
                                 const EImageLayout initLayout,
                                 const EImageLayout finalLayout);

    private:
        void _build();
        void _topoSortRecursive(SubPass* root, vector<vector<SubPass*>>& nodes,
                                vector<bool>& visited,
                                linked_list<SubPass*>& stack);

        std::unordered_map<u32, RenderPassBuilder::AttachmentDescription>
            _attachments;
        RenderGraph& _parent;
        i32 _id;
        IRenderPass* _pass;
        std::string _name;
        vector<SubPass*> _passes;
    };

    class SubPass
    {
        friend class RenderPass;

        SubPass(RenderPass& parent, const std::string& name, const i32 id);

    public:
        bool addColorOutputAttachment(const std::string& name);
        bool addInputAttachment(const std::string& name);
        bool addDepthStencilAttachment(const std::string& name);

        bool dependsOn(const std::string& pass,
                       const PassDependencyInfo& dependencyInfo);

    private:
        RenderPass& _parent;
        i32 _id;
        std::string _name;
        vector<i32> _dependsOn;
        vector<PassDependencyInfo> _dependencyInfos;

        vector<u32> _colorOutputs;
        vector<u32> _inputAttachments;
        i32 _depthAttachment;
    };

    class RenderGraph
    {
        friend class RenderPass;
        friend class SubPass;

    public:
        RenderGraph(IDevice* device, ISwapchain* swapchain);
        ~RenderGraph();

        HELIOS_NO_COPY_MOVE(RenderGraph);

        bool addImage(const ImageInfo& info);
        bool addAttachment(const std::string& name,
                           const TextureAttachmentResource& attachment);

        RenderPass& addGraphicsPass(const std::string& name);
        void build();
        void releaseResources();

    private:
        IDevice* _device;
        ISwapchain* _swapchain;

        vector<IImage*> _images;
        vector<IImageView*> _views;

        std::unordered_map<std::string, u32> _imageResources;
        std::unordered_map<std::string, u32> _viewResources;
        std::unordered_map<u32, ImageInfo> _imageInfos;
        std::unordered_map<u32, TextureAttachmentResource> _viewInfos;

        vector<RenderPass*> _passes;
    };
} // namespace helios
