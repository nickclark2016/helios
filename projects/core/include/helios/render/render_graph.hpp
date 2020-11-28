#pragma once

#include <helios/containers/optional.hpp>
#include <helios/containers/stl_hashes.hpp>
#include <helios/containers/unordered_map.hpp>
#include <helios/macros.hpp>
#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>
#include <helios/render/shader.hpp>

#include <string>

namespace helios
{
    class RenderPass;
    class RenderGraph;

    struct RelativeDimension final
    {
        f32 width;
        f32 height;
        f32 depth;
    };

    struct AbsoluteDimension final
    {
        u32 width;
        u32 height;
        u32 depth;
    };

    enum class EDimensionType
    {
        ABSOLUTE,
        RELATIVE
    };

    struct ImageDimension final
    {
        union
        {
            AbsoluteDimension absolute;
            RelativeDimension relative;
        };
        EDimensionType dimension;
    };

    struct SamplerResourceInfo final
    {
        EFilter minification = EFilter::LINEAR;
        EFilter magnification = EFilter::LINEAR;
        ESamplerMipMapMode mipMode = ESamplerMipMapMode::LINEAR;
        ESamplerAddressMode addressModeU = ESamplerAddressMode::REPEAT;
        ESamplerAddressMode addressModeV = ESamplerAddressMode::REPEAT;
        ESamplerAddressMode addressModeW = ESamplerAddressMode::REPEAT;
        f32 mipLevelOfDetailBias = 0.0f;
        optional<f32> anisotropy;
        optional<ECompareOp> compareOp;
        f32 minLevelOfDetail = 0.0f;
        f32 maxLevelOfDetail = 1.0f;
        EBorderColor borderColor = EBorderColor::INT_OPAQUE_BLACK;
        bool unnormalizedCoordinates = false;
    };

    struct ImageResourceInfo final
    {
        bool perFrameInFlightResource;
        EFormat format;
        ImageDimension dimensions;
        EImageType type;
        EImageViewType viewType;
        u32 mips;
        u32 layers;
        ESampleCountFlagBits samples;
        EImageTiling tiling;
        EImageUsageFlags usage;
        EImageLayout layout;
        EImageAspectFlags aspect;
        optional<SamplerResourceInfo> sampler;
    };

    struct BufferResourceInfo final
    {
        bool perFrameInFlightResource;
        u32 size;
        bool transferSrc;
        bool transferDst;
        bool deviceLocal;
        bool hostVisible;
        bool hostCached;
        bool hostCoherent;
    };

    class ImageResource final
    {
        friend class RenderGraph;
        ImageResource() = default;
    public:
        ~ImageResource();
        HELIOS_NO_COPY_MOVE(ImageResource)

        IImage& image(u32 frame = 0);
        IImageView& view(u32 frame = 0);
        ISampler& sampler();
        std::string name() const;
        ImageResourceInfo info() const noexcept;
        EFormat format() const noexcept;
        ESampleCountFlagBits samples() const noexcept;

    private:
        vector<IImage*> _image;
        ImageResourceInfo _info;
        std::string _name;
        ISampler* _sampler;
        vector<IImageView*> _view;
        EFormat _fmt;
        ESampleCountFlagBits _samples;
    };

    class BufferResource final
    {
        friend class RenderGraph;
        BufferResource() = default;
    public:
        ~BufferResource();
        HELIOS_NO_COPY_MOVE(BufferResource)

        IBuffer& buffer(const u32 frame = 0);
        BufferResourceInfo info() const;
        std::string name() const;

    private:
        vector<IBuffer*> _buffers;
        BufferResourceInfo _info;
        std::string _name;
    };

    struct ImageAccessInfo
    {
        EImageLayout layout;
        EAccessFlags readMask;
        EAccessFlags writeMask;
        EPipelineStageFlags readStages;
        EPipelineStageFlags writeStages;
        EAttachmentLoadOp loadOp;
        EAttachmentStoreOp storeOp;
        EAttachmentLoadOp stencilLoadOp;
        EAttachmentStoreOp stencilStoreOp;
    };

    struct BufferAccessInfo
    {
        EAccessFlags readMask;
        EAccessFlags writeMask;
        EPipelineStageFlags readStages;
        EPipelineStageFlags writeStages;
        u64 offset;
        u64 size;
    };

    class RenderPass
    {
        struct ShaderInfo
        {
            std::string vertex;
            std::string fragment;
        };

        friend class RenderGraph;
        RenderPass() = default;
    public:
        ~RenderPass();
        HELIOS_NO_COPY_MOVE(RenderPass)

        RenderPass& addColorAttachment(const std::string& name, const ImageAccessInfo& access);
        RenderPass& addDepthAttachment(const std::string& name, const ImageAccessInfo& access);
        RenderPass& addInputAttachment(const std::string& name, const ImageAccessInfo& access);
        RenderPass& addUniformBuffer(const std::string& name, const BufferAccessInfo& access);
        RenderPass& succeeds(const std::string& pass);
        RenderPass& preceeds(const std::string& pass);
        RenderPass& attachShader(const std::string& name, const std::string& vertex, const std::string& fragment);

        std::string name() const;

    private:
        RenderGraph* _graph; // non-owning
        std::string _name;
        IRenderPass* _pass; // owning
        vector<IFramebuffer*> _renderTargets; // owning

        // Mapping of attachment names to resource handles
        unordered_map<std::string, ImageResource*> _colorAttachments; // non-owning
        ImageResource* _depthAttachment = nullptr; // non-owning
        unordered_map<std::string, ImageResource*> _inputAttachments; // non-owning
        unordered_map<std::string, BufferResource*> _uniformBuffers; // non-owning
        unordered_map<std::string, Shader*> _shaders; // owning

        // Mapping of attachments names to access information
        unordered_map<std::string, ImageAccessInfo> _colorAttachmentInfos;
        ImageAccessInfo _depthAttachmentInfo;
        unordered_map<std::string, ImageAccessInfo> _inputAttachmentInfos;
        unordered_map<std::string, BufferAccessInfo> _uniformBufferInfos;
        unordered_map<std::string, ShaderInfo> _shaderInfos;

        vector<std::string> _succeeds;
        vector<std::string> _preceeds;

        bool _build();
    };

	class RenderGraph
    {
        friend class RenderPass;
    public:
        RenderGraph() = default;
        ~RenderGraph();
        HELIOS_NO_COPY_MOVE(RenderGraph);

        ImageResource& addImageResource(const std::string& name, const ImageResourceInfo& info);
        BufferResource& addUniformBufferResource(const std::string& name, const BufferResourceInfo& info);
        RenderPass& createPass(const std::string& name);

        bool build();

    private:
        unordered_map<std::string, ImageResource*> _images; // owning
        unordered_map<std::string, BufferResource*> _uniforms; // owning
        unordered_map<std::string, RenderPass*> _renderPasses; // owning

        ImageResource* _getImageByName(const std::string& name);
        BufferResource* _getBufferByName(const std::string& name);
    };
} // namespace helios