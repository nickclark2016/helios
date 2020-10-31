#pragma once

#include <helios/containers/optional.hpp>
#include <helios/containers/stl_hashes.hpp>
#include <helios/containers/unordered_map.hpp>
#include <helios/macros.hpp>
#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

#include <string_view>

namespace helios
{
    class SubPass;
    class Pass;
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
        std::string_view name() const;
        ImageResourceInfo info() const noexcept;

    private:
        vector<IImage*> _image;
        ImageResourceInfo _info;
        std::string _name;
        ISampler* _sampler;
        vector<IImageView*> _view;
    };

    class BufferResource final
    {
        friend class RenderGraph;
        BufferResource() = default;
    public:
        ~BufferResource();
        HELIOS_NO_COPY_MOVE(BufferResource)

        IBuffer& buffer();
        BufferResourceInfo info() const;
        std::string_view name() const;

    private:
        IBuffer* _buffer;
        BufferResourceInfo _info;
        std::string _name;
    };

	class RenderGraph
    {
    public:
        RenderGraph() = default;
        HELIOS_NO_COPY_MOVE(RenderGraph);

        ~RenderGraph();
        ImageResource& addImageResource(std::string_view name, const ImageResourceInfo& info);

    private:
        unordered_map<std::string, ImageResource*> _image;
    };
} // namespace helios