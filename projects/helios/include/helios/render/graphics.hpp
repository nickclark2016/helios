#pragma once

#include <helios/containers/optional.hpp>
#include <helios/containers/vector.hpp>
#include <helios/core/window.hpp>
#include <helios/macros.hpp>
#include <helios/render/enums.hpp>

#include <functional>
#include <string>

namespace helios
{
    union ClearColorValue {
        f32 float32[4];
        i32 int32[4];
        u32 uint32[4];
    };

    struct ClearDepthStencilValue
    {
        f32 depth;
        u32 stencil;
    };

    union ClearValue {
        ClearColorValue color;
        ClearDepthStencilValue depthStencil;
    };

    class ICommandBuffer;
    class ICommandPool;
    class IContext;
    class IDescriptorSetLayout;
    class IDevice;
    class IFence;
    class IFramebuffer;
    class IGraphicsPipeline;
    class IImage;
    class IImageView;
    class IPhysicalDevice;
    class IPipelineLayout;
    class IQueue;
    class IRenderPass;
    class ISemaphore;
    class IShaderModule;
    class ISurface;
    class ISwapchain;

    struct QueueFamilyInfo
    {
        u32 family;
        bool protect = false;
        vector<float> priorities;
    };

    struct DescriptorSetLayoutBinding
    {
        u32 binding;
        EDescriptorType type;
        u32 count;
        EShaderStageFlags stages;
    };

    struct PushConstantRange
    {
        EShaderStageFlags stages;
        u32 offset;
        u32 size;
    };

    class ContextBuilder final
    {
    public:
        ContextBuilder();
        ContextBuilder(const ContextBuilder&) = delete;
        ContextBuilder(ContextBuilder&&) noexcept = delete;
        ~ContextBuilder();
        ContextBuilder& operator=(const ContextBuilder&) = delete;
        ContextBuilder& operator=(ContextBuilder&&) noexcept = delete;

        ContextBuilder& applicationName(const std::string& name);
        ContextBuilder& applicationVersion(const u32 major, const u32 minor,
                                           const u32 revision);
        ContextBuilder& engineName(const std::string& name);
        ContextBuilder& engineVersion(const u32 major, const u32 minor,
                                      const u32 revision);
        ContextBuilder& validation(
            std::function<void(EMessageSeverity, std::string)> callback =
                DefaultCallback);
        IContext* build() const;

    private:
        struct ContextBuilderImpl;

        ContextBuilderImpl* _impl;
        static std::function<void(EMessageSeverity, const std::string)>
            DefaultCallback;
    };

    class IContext
    {
    protected:
        IContext() = default;

    public:
        virtual ~IContext() = default;

        [[nodiscard]] virtual vector<IPhysicalDevice*> physicalDevices()
            const = 0;

        IContext(const IContext&) = delete;
        IContext(IContext&&) noexcept = delete;
        IContext& operator=(const IContext&) = delete;
        IContext& operator=(IContext&&) noexcept = delete;
    };

    class IPhysicalDevice
    {
    protected:
        IPhysicalDevice() = default;

    public:
        struct Features
        {
            bool robustBufferAccess;
            bool fullDrawUint32;
            bool imageCubeArray;
            bool independentBlend;
            bool geometryShader;
            bool tessellationShader;
            bool sampleRateShading;
            bool dualSourceBlend;
            bool logicOperations;
            bool multidrawIndirect;
            bool drawIndirectFirstInstance;
            bool depthClamp;
            bool depthBiasClamp;
            bool fillModeNonSolid;
            bool depthBounds;
            bool wideLines;
            bool largePoints;
            bool alphaToOne;
            bool multiViewport;
            bool samplerAnisotropy;
            bool textureCompressionEtc2;
            bool textureCompressionAstcLdr;
            bool textureCompressionBc;
            bool occlusionQueryPrecise;
            bool pipelineStatisticsQuery;
            bool vertexPipelineStoresAndAtomics;
            bool fragmentStoresAndAtomics;
            bool shaderTessellationAndGeometryPointSize;
            bool shaderImageGatherExtended;
            bool shaderStorageImageExtendedFormats;
            bool shaderStorageImageMultisample;
            bool shaderStorageImageReadWithoutFormat;
            bool shaderStorageImageWriteWithoutFormat;
            bool shaderUniformBufferArrayDynamicIndexing;
            bool shaderSampledImageArrayDynamicIndexing;
            bool shaderStorageBufferArrayDynamicIndexing;
            bool shaderStorageImageArrayDynamicIndexing;
            bool shaderClipDistance;
            bool shaderCullDistance;
            bool shaderFloat64;
            bool shaderInt64;
            bool shaderInt16;
            bool shaderResourceResidency;
            bool shaderResourceMinLod;
            bool sparseBinding;
            bool sparseResidencyBuffer;
            bool sparseResidencyImage2D;
            bool sparseResidencyImage3D;
            bool sparseResidency2Samples;
            bool sparseResidency4Samples;
            bool sparseResidency8Samples;
            bool sparseResidency16Samples;
            bool sparseResidencyAliased;
            bool variableMultisampleRate;
            bool inheritedQueries;
        };

        struct QueueProperties
        {
            u32 index;
            bool graphics;
            bool compute;
            bool transfer;
            bool sparse;
            bool protect;
            u32 count;
        };

        virtual ~IPhysicalDevice() = default;
        [[nodiscard]] virtual std::string name() const = 0;
        [[nodiscard]] virtual EPhysicalDeviceType type() const = 0;
        [[nodiscard]] virtual Features features() const = 0;
        [[nodiscard]] virtual vector<QueueProperties> queueProperties() = 0;

        IPhysicalDevice(const IPhysicalDevice&) = delete;
        IPhysicalDevice(IPhysicalDevice&&) noexcept = delete;
        IPhysicalDevice& operator=(const IPhysicalDevice&) = delete;
        IPhysicalDevice& operator=(IPhysicalDevice&&) noexcept = delete;
    };

    class DeviceBuilder
    {
    public:
        DeviceBuilder();
        DeviceBuilder(const DeviceBuilder&) = delete;
        DeviceBuilder(DeviceBuilder&&) noexcept = delete;
        ~DeviceBuilder();
        DeviceBuilder& operator=(const DeviceBuilder&) = delete;
        DeviceBuilder& operator=(DeviceBuilder&&) noexcept = delete;

        DeviceBuilder& queues(const vector<QueueFamilyInfo>& infos);
        DeviceBuilder& features(const IPhysicalDevice::Features& features);
        DeviceBuilder& validation();
        DeviceBuilder& swapchain();
        DeviceBuilder& physical(const IPhysicalDevice* pPhysical);
        DeviceBuilder& compute(const u32 count);
        DeviceBuilder& transfer(const u32 count);
        DeviceBuilder& graphics(const u32 count);
        [[nodiscard]] IDevice* build() const;

    private:
        struct DeviceBuilderImpl;

        DeviceBuilderImpl* _impl;
    };

    class IQueue
    {
    protected:
        IQueue() = default;

    public:
        struct SubmitInfo
        {
            vector<ISemaphore*> wait;
            vector<EPipelineStageFlags> waitMask;
            vector<ISemaphore*> signal;
            vector<ICommandBuffer*> buffers;
        };

        struct PresentInfo
        {
            vector<ISemaphore*> waits;
            ISwapchain* swapchain;
            u32 image;
        };

        IQueue(const IQueue&) = delete;
        IQueue(IQueue&&) noexcept = delete;
        virtual ~IQueue() = default;
        IQueue& operator=(const IQueue&) = delete;
        IQueue& operator=(IQueue&&) noexcept = delete;

        [[nodiscard]] virtual float priority() const = 0;
        [[nodiscard]] virtual u32 index() = 0;
        [[nodiscard]] virtual IPhysicalDevice::QueueProperties props()
            const = 0;
        [[nodiscard]] virtual bool canPresent(
            const IPhysicalDevice* device, const ISurface* surface) const = 0;
        virtual void submit(const vector<SubmitInfo>& submitInfo,
                            const IFence* signal) const = 0;
        virtual void present(const PresentInfo& presentInfo) const = 0;
    };

    class IDevice
    {
    protected:
        IDevice() = default;

    public:
        virtual ~IDevice() = default;

        [[nodiscard]] virtual vector<IQueue*> queues() const = 0;
        virtual void idle() const = 0;

        IDevice(const IDevice&) = delete;
        IDevice(IDevice&&) noexcept = delete;
        IDevice& operator=(const IDevice&) = delete;
        IDevice& operator=(IDevice&&) noexcept = delete;
    };

    class SurfaceBuilder
    {
    public:
        SurfaceBuilder();
        ~SurfaceBuilder();

        SurfaceBuilder& device(const IDevice* device);
        SurfaceBuilder& window(const IWindow* window);
        [[nodiscard]] ISurface* build() const;

        SurfaceBuilder(const SurfaceBuilder&) = delete;
        SurfaceBuilder(SurfaceBuilder&&) noexcept = delete;
        SurfaceBuilder& operator=(const SurfaceBuilder&) = delete;
        SurfaceBuilder& operator=(SurfaceBuilder&&) noexcept = delete;

    private:
        struct SurfaceBuilderImpl;
        SurfaceBuilderImpl* _impl;
    };

    class ISurface
    {
    protected:
        ISurface() = default;

    public:
        struct SurfaceFormat
        {
            EFormat format;
            EColorSpace colorSpace;
        };

        struct SwapchainSupport
        {
            u32 minImages;
            u32 maxImages;
            u32 currentWidth;
            u32 currentHeight;
            u32 minWidth;
            u32 minHeight;
            u32 maxWidth;
            u32 maxHeight;
            u32 maxImageArrayLayers;
            bool alphaOpaque;
            bool alphaPremultiplied;
            bool alphaPostmultiplied;
            bool alphaInherit;
            EImageUsageFlags supportedUsage;
            ESurfaceTransformFlagBits currentTransform;
            ESurfaceTransformFlags supportedTransforms;

            vector<SurfaceFormat> surfaceFormats;
            vector<EPresentMode> presentModes;
        };

        virtual ~ISurface() = default;
        [[nodiscard]] virtual IWindow* window() const = 0;
        [[nodiscard]] virtual SwapchainSupport swapchainSupport(
            const IPhysicalDevice* device) const = 0;

        ISurface(const ISurface&) = delete;
        ISurface(ISurface&&) noexcept = delete;
        ISurface& operator=(const ISurface&) = delete;
        ISurface& operator=(ISurface&&) noexcept = delete;
    };

    class SwapchainBuilder
    {
    public:
        SwapchainBuilder();
        ~SwapchainBuilder();

        SwapchainBuilder& surface(const ISurface* surface);
        SwapchainBuilder& images(const u32 count);
        SwapchainBuilder& width(const u32 width);
        SwapchainBuilder& height(const u32 height);
        SwapchainBuilder& layers(const u32 layers);
        SwapchainBuilder& previous(const ISwapchain* previous);
        SwapchainBuilder& present(const EPresentMode present);
        SwapchainBuilder& format(const EFormat format);
        SwapchainBuilder& colorSpace(const EColorSpace colorSpace);
        SwapchainBuilder& queues(const vector<IQueue*>& queues);
        SwapchainBuilder& usage(const EImageUsageFlags usage);
        SwapchainBuilder& transform(const ESurfaceTransformFlagBits transform);
        SwapchainBuilder& alphaOpaque();
        SwapchainBuilder& alphaPremultiply();
        SwapchainBuilder& alphaPostmultiply();
        SwapchainBuilder& alphaInherit();
        SwapchainBuilder& clipped();
        [[nodiscard]] ISwapchain* build() const;

        SwapchainBuilder(const SwapchainBuilder&) = delete;
        SwapchainBuilder(SwapchainBuilder&&) noexcept = delete;
        SwapchainBuilder& operator=(const SwapchainBuilder&) = delete;
        SwapchainBuilder& operator=(SwapchainBuilder&&) noexcept = delete;

    private:
        struct SwapchainBuilderImpl;

        SwapchainBuilderImpl* _impl;
    };

    class ISwapchain
    {
    protected:
        ISwapchain() = default;

    public:
        virtual ~ISwapchain() = default;

        [[nodiscard]] virtual u32 imagesCount() const = 0;
        [[nodiscard]] virtual vector<IImageView*> views() const = 0;
        [[nodiscard]] virtual EFormat format() const = 0;
        [[nodiscard]] virtual u32 acquireNextImage(const u64 wait,
                                                   const ISemaphore* signal,
                                                   const IFence* fence) = 0;

        ISwapchain(const ISwapchain&) = delete;
        ISwapchain(ISwapchain&&) noexcept = delete;
        ISwapchain& operator=(const ISwapchain&) = delete;
        ISwapchain& operator=(ISwapchain&&) noexcept = delete;
    };

    class ImageBuilder
    {
    public:
        ImageBuilder();
        ~ImageBuilder();

        ImageBuilder(const ImageBuilder&) = delete;
        ImageBuilder(ImageBuilder&&) noexcept = delete;
        ImageBuilder& operator=(const ImageBuilder&) = delete;
        ImageBuilder& operator=(ImageBuilder&&) noexcept = delete;

    private:
        struct ImageBuilderImpl;

        ImageBuilderImpl* _impl;
    };

    class IImage
    {
    protected:
        IImage() = default;

    public:
        virtual ~IImage() = default;

        IImage(const IImage&) = delete;
        IImage(IImage&&) noexcept = delete;
        IImage& operator=(const IImage&) = delete;
        IImage& operator=(IImage&&) noexcept = delete;
    };

    class ImageViewBuilder
    {
    public:
        ImageViewBuilder();
        ~ImageViewBuilder();

        ImageViewBuilder& type(const EImageViewType type);
        ImageViewBuilder& image(const IImage* image);
        ImageViewBuilder& format(const EFormat format);
        ImageViewBuilder& redMapping(const EComponentSwizzle red);
        ImageViewBuilder& greenMapping(const EComponentSwizzle green);
        ImageViewBuilder& blueMapping(const EComponentSwizzle blue);
        ImageViewBuilder& alphaMapping(const EComponentSwizzle alpha);
        ImageViewBuilder& aspect(const EImageAspectFlags aspect);
        ImageViewBuilder& baseMipLevel(const u32 base);
        ImageViewBuilder& mipLevels(const u32 count);
        ImageViewBuilder& baseArrayLayer(const u32 base);
        ImageViewBuilder& arrayLayers(const u32 count);
        [[nodiscard]] IImageView* build() const;

        ImageViewBuilder(const ImageViewBuilder&) = delete;
        ImageViewBuilder(ImageViewBuilder&&) noexcept = delete;
        ImageViewBuilder& operator=(const ImageViewBuilder&) = delete;
        ImageViewBuilder& operator=(ImageViewBuilder&&) noexcept = delete;

    private:
        struct ImageViewBuilderImpl;

        ImageViewBuilderImpl* _impl;
    };

    class IImageView
    {
    protected:
        IImageView() = default;

    public:
        virtual ~IImageView() = default;

        IImageView(const IImageView&) = delete;
        IImageView(IImageView&&) noexcept = delete;
        IImageView& operator=(const IImageView&) = delete;
        IImageView& operator=(IImageView&&) noexcept = delete;
    };

    class ShaderModuleBuilder
    {
    public:
        ShaderModuleBuilder();
        ~ShaderModuleBuilder();

        ShaderModuleBuilder& device(const IDevice* device);
        ShaderModuleBuilder& source(const vector<u8>& source);
        [[nodiscard]] IShaderModule* build() const;

        ShaderModuleBuilder(const ShaderModuleBuilder&) = delete;
        ShaderModuleBuilder(ShaderModuleBuilder&&) noexcept = delete;
        ShaderModuleBuilder& operator=(const ShaderModuleBuilder&) = delete;
        ShaderModuleBuilder& operator=(ShaderModuleBuilder&&) noexcept = delete;

    private:
        struct ShaderModuleBuilderImpl;

        ShaderModuleBuilderImpl* _impl;
    };

    class IShaderModule
    {
    protected:
        IShaderModule() = default;

    public:
        virtual ~IShaderModule() = default;

        IShaderModule(const IShaderModule&) = delete;
        IShaderModule(IShaderModule&&) noexcept = delete;
        IShaderModule& operator=(const IShaderModule&) = delete;
        IShaderModule& operator=(IShaderModule&&) noexcept = delete;
    };

    class DescriptorSetLayoutBuilder
    {
    public:
        DescriptorSetLayoutBuilder();
        ~DescriptorSetLayoutBuilder();

        DescriptorSetLayoutBuilder& device(const IDevice* device);
        DescriptorSetLayoutBuilder& bindings(
            const vector<DescriptorSetLayoutBinding>& bindings);
        [[nodiscard]] IDescriptorSetLayout* build() const;

        DescriptorSetLayoutBuilder(const DescriptorSetLayoutBuilder&) = delete;
        DescriptorSetLayoutBuilder(DescriptorSetLayoutBuilder&&) noexcept =
            delete;
        DescriptorSetLayoutBuilder& operator=(
            const DescriptorSetLayoutBuilder&) = delete;
        DescriptorSetLayoutBuilder& operator=(
            DescriptorSetLayoutBuilder&&) noexcept = delete;

    private:
        struct DescriptorSetLayoutBuilderImpl;

        DescriptorSetLayoutBuilderImpl* _impl;
    };

    class IDescriptorSetLayout
    {
    protected:
        IDescriptorSetLayout() = default;

    public:
        virtual ~IDescriptorSetLayout() = default;

        [[nodiscard]] virtual vector<DescriptorSetLayoutBinding> bindings()
            const = 0;

        IDescriptorSetLayout(const IDescriptorSetLayout&) = delete;
        IDescriptorSetLayout(IDescriptorSetLayout&&) noexcept = delete;
        IDescriptorSetLayout& operator=(const IDescriptorSetLayout&) = delete;
        IDescriptorSetLayout& operator=(IDescriptorSetLayout&&) noexcept =
            delete;
    };

    class PipelineLayoutBuilder
    {
    public:
        PipelineLayoutBuilder();
        ~PipelineLayoutBuilder();

        PipelineLayoutBuilder& device(const IDevice* device);
        PipelineLayoutBuilder& layouts(
            const vector<IDescriptorSetLayout*>& layouts);
        PipelineLayoutBuilder& pushConstants(
            const vector<PushConstantRange>& ranges);
        [[nodiscard]] IPipelineLayout* build() const;

        PipelineLayoutBuilder(const PipelineLayoutBuilder&) = delete;
        PipelineLayoutBuilder(PipelineLayoutBuilder&&) noexcept = delete;
        PipelineLayoutBuilder& operator=(const PipelineLayoutBuilder&) = delete;
        PipelineLayoutBuilder& operator=(PipelineLayoutBuilder&&) noexcept =
            delete;

    private:
        struct PipelineLayoutBuilderImpl;

        PipelineLayoutBuilderImpl* _impl;
    };

    class IPipelineLayout
    {
    protected:
        IPipelineLayout() = default;

    public:
        virtual ~IPipelineLayout() = default;

        IPipelineLayout(const IPipelineLayout&) = delete;
        IPipelineLayout(IPipelineLayout&&) noexcept = delete;
        IPipelineLayout& operator=(const IPipelineLayout&) = delete;
        IPipelineLayout& operator=(IPipelineLayout&&) noexcept = delete;
    };

    class GraphicsPipelineBuilder
    {
    public:
        struct VertexInputAttributeBinding
        {
            u32 binding;
            u32 stride;
            EVertexInputRate inputRate;
            u32 location;
            EFormat format;
            u32 offset;
        };

        struct VertexInputState
        {
            vector<VertexInputAttributeBinding> bindings;
        };

        struct InputAssemblyState
        {
            EPrimitiveTopology topology;
            bool primitiveRestartEnable;
        };

        struct TessellationState
        {
            u32 patchControlPoints;
        };

        struct Viewport
        {
            f32 x;
            f32 y;
            f32 width;
            f32 height;
            f32 minDepth;
            f32 maxDepth;
        };

        struct Rect2D
        {
            i32 x;
            i32 y;
            u32 width;
            u32 height;
        };

        struct ViewportState
        {
            vector<Viewport> viewports;
            vector<Rect2D> scissorRegions;
        };

        struct RasterizationState
        {
            bool depthClampEnable = false;
            bool rasterizerDiscardEnable = true;
            EPolygonMode mode = EPolygonMode::FILL;
            ECullModeFlags cullMode = CULL_MODE_BACK;
            EVertexWindingOrder frontFace =
                EVertexWindingOrder::COUNTER_CLOCKWISE;
            bool depthBiasEnable = false;
            f32 depthBiasConstantFactor = 0.0f;
            f32 depthBiasClamp = 0.0f;
            f32 depthBiasSlopeFactor = 0.0f;
            f32 lineWidth = 1.0f;
        };

        struct MultisampleState
        {
            ESampleCountFlagBits samples = SAMPLE_COUNT_1;
            bool sampleShadingEnable = false;
            f32 minSampleShading = 0.0f;
            optional<u32> sampleMask;
            bool alphaToCoverage = false;
            bool alphaToOne = false;
        };

        struct StencilOpState
        {
            EStencilOp failOp;
            EStencilOp passOp;
            EStencilOp depthFailOp;
            ECompareOp compareOp;
            u32 compareMask;
            u32 writeMask;
            u32 reference;
        };

        struct DepthStencilState
        {
            bool depthTestEnable = false;
            bool depthWriteEnable = false;
            ECompareOp depthCompareOp = ECompareOp::LESS;
            bool depthBoundsTestEnable = false;
            bool stencilTestEnable = false;
            StencilOpState front = {};
            StencilOpState back = {};
            f32 minDepthBounds = 0.0f;
            f32 maxDepthBounds = 1.0f;
        };

        struct ColorBlendAttachmentState
        {
            bool blendEnable = false;
            EBlendFactor srcColorBlendFactor = EBlendFactor::SRC_COLOR;
            EBlendFactor dstColorBlendFactor =
                EBlendFactor::ONE_MINUS_CONSTANT_COLOR;
            EBlendOp colorBlendOp = EBlendOp::ADD;
            EBlendFactor srcAlphaBlendFactor = EBlendFactor::SRC_ALPHA;
            EBlendFactor dstAlphaBlendFactor =
                EBlendFactor::ONE_MINUS_SRC_ALPHA;
            EBlendOp alphaBlendOp = EBlendOp::ADD;
            EColorComponentFlags colorMask =
                COLOR_COMPONENT_R | COLOR_COMPONENT_G | COLOR_COMPONENT_B |
                COLOR_COMPONENT_A;
        };

        struct ColorBlendState
        {
            bool logicEnable = false;
            ELogicOp logicOp = ELogicOp::CLEAR;
            vector<ColorBlendAttachmentState> attachments;
            f32 blendConstants[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        };

        struct DynamicState
        {
            vector<EDynamicState> states;
        };

        GraphicsPipelineBuilder();
        ~GraphicsPipelineBuilder();

        GraphicsPipelineBuilder& allowDerivatives();
        GraphicsPipelineBuilder& deriveFrom(const IGraphicsPipeline* parent);
        GraphicsPipelineBuilder& vertex(const IShaderModule* vertex);
        GraphicsPipelineBuilder& fragment(const IShaderModule* fragment);
        GraphicsPipelineBuilder& input(const VertexInputState& bindings);
        GraphicsPipelineBuilder& assembly(const InputAssemblyState& assembly);
        GraphicsPipelineBuilder& tessellation(
            const TessellationState& tessellation);
        GraphicsPipelineBuilder& viewports(const ViewportState& viewport);
        GraphicsPipelineBuilder& rasterization(
            const RasterizationState& raster);
        GraphicsPipelineBuilder& multisample(const MultisampleState& multi);
        GraphicsPipelineBuilder& depthStencil(const DepthStencilState& depth);
        GraphicsPipelineBuilder& colorBlend(const ColorBlendState& color);
        GraphicsPipelineBuilder& dynamic(const DynamicState& dynamic);
        GraphicsPipelineBuilder& layout(const IPipelineLayout* layout);
        GraphicsPipelineBuilder& subpass(const u32 subpass);
        GraphicsPipelineBuilder& renderpass(const IRenderPass* renderpass);
        [[nodiscard]] IGraphicsPipeline* build() const;

        GraphicsPipelineBuilder(const GraphicsPipelineBuilder&) = delete;
        GraphicsPipelineBuilder(GraphicsPipelineBuilder&&) noexcept = delete;
        GraphicsPipelineBuilder& operator=(const GraphicsPipelineBuilder&) =
            delete;
        GraphicsPipelineBuilder& operator=(GraphicsPipelineBuilder&&) noexcept =
            delete;

    private:
        struct GraphicsPipelineBuilderImpl;

        GraphicsPipelineBuilderImpl* _impl;
    };

    class IGraphicsPipeline
    {
    protected:
        IGraphicsPipeline() = default;

    public:
        virtual ~IGraphicsPipeline() = default;

        IGraphicsPipeline(const IGraphicsPipeline&) = delete;
        IGraphicsPipeline(IGraphicsPipeline&&) noexcept = delete;
        IGraphicsPipeline& operator=(const IGraphicsPipeline&) = delete;
        IGraphicsPipeline& operator=(IGraphicsPipeline&&) noexcept = delete;
    };

    class RenderPassBuilder
    {
    public:
        struct AttachmentDescription
        {
            EFormat format;
            ESampleCountFlagBits samples;
            EAttachmentLoadOp loadOp;
            EAttachmentStoreOp storeOp;
            EAttachmentLoadOp stencilLoadOp;
            EAttachmentStoreOp stencilStoreOp;
            EImageLayout initialLayout;
            EImageLayout finalLayout;
        };

        struct AttachmentReference
        {
            u32 attachment;
            EImageLayout layout;
        };

        struct SubpassDescription
        {
            EBindPoint bind;
            vector<AttachmentReference> inputAttachments;
            vector<AttachmentReference> colorAttachments;
            vector<AttachmentReference> resolveAttachments;
            optional<AttachmentReference> depthAttachment;
            vector<u32> preserveAttachments;
        };

        struct SubpassDependency
        {
            u32 srcSubpass;
            u32 dstSubpass;
            EPipelineStageFlags srcStageMask;
            EPipelineStageFlags dstStageMask;
            EAccessFlags srcAccessMask;
            EAccessFlags dstAccessMask;
            EDependencyFlags dependencyFlags;
        };

        RenderPassBuilder();
        ~RenderPassBuilder();

        RenderPassBuilder& device(const IDevice* device);
        RenderPassBuilder& attachments(
            const vector<AttachmentDescription>& attachments);
        RenderPassBuilder& subpasses(
            const vector<SubpassDescription>& subpasses);
        RenderPassBuilder& dependencies(
            const vector<SubpassDependency>& dependencies);
        [[nodiscard]] IRenderPass* build() const;

        RenderPassBuilder(const RenderPassBuilder&) = delete;
        RenderPassBuilder(RenderPassBuilder&&) noexcept = delete;
        RenderPassBuilder& operator=(const RenderPassBuilder&) = delete;
        RenderPassBuilder& operator=(RenderPassBuilder&&) noexcept = delete;

    private:
        struct RenderPassBuilderImpl;

        RenderPassBuilderImpl* _impl;
    };

    class IRenderPass
    {
    protected:
        IRenderPass() = default;

    public:
        virtual ~IRenderPass() = default;

        IRenderPass(const IRenderPass&) = delete;
        IRenderPass(IRenderPass&&) noexcept = delete;
        IRenderPass& operator=(const IRenderPass&) = delete;
        IRenderPass& operator=(IRenderPass&&) noexcept = delete;
    };

    class FramebufferBuilder
    {
    public:
        FramebufferBuilder();
        ~FramebufferBuilder();

        FramebufferBuilder& renderpass(const IRenderPass* renderpass);
        FramebufferBuilder& attachments(const vector<IImageView*>& attachments);
        FramebufferBuilder& width(const u32 width);
        FramebufferBuilder& height(const u32 height);
        FramebufferBuilder& layers(const u32 layers);
        [[nodiscard]] IFramebuffer* build() const;

        FramebufferBuilder(const FramebufferBuilder&) = delete;
        FramebufferBuilder(FramebufferBuilder&&) noexcept = delete;
        FramebufferBuilder& operator=(const FramebufferBuilder&) = delete;
        FramebufferBuilder& operator=(FramebufferBuilder&&) noexcept = delete;

    private:
        struct FramebufferBuilderImpl;

        FramebufferBuilderImpl* _impl;
    };

    class IFramebuffer
    {
    protected:
        IFramebuffer() = default;

    public:
        virtual ~IFramebuffer() = default;

        [[nodiscard]] virtual u32 width() const = 0;
        [[nodiscard]] virtual u32 height() const = 0;
        [[nodiscard]] virtual u32 layers() const = 0;
        [[nodiscard]] virtual vector<IImageView*> attachments() const = 0;

        IFramebuffer(const IFramebuffer&) = delete;
        IFramebuffer(IFramebuffer&&) noexcept = delete;
        IFramebuffer& operator=(const IFramebuffer&) = delete;
        IFramebuffer& operator=(IFramebuffer&&) noexcept = delete;
    };

    class CommandPoolBuilder
    {
    public:
        CommandPoolBuilder();
        ~CommandPoolBuilder();

        CommandPoolBuilder& device(const IDevice* device);
        CommandPoolBuilder& queue(const IQueue* queue);
        CommandPoolBuilder& transient();
        CommandPoolBuilder& reset();
        [[nodiscard]] ICommandPool* build() const;

        CommandPoolBuilder(const CommandPoolBuilder&) = delete;
        CommandPoolBuilder(CommandPoolBuilder&&) noexcept = delete;
        CommandPoolBuilder& operator=(const CommandPoolBuilder&) = delete;
        CommandPoolBuilder& operator=(CommandPoolBuilder&&) noexcept = delete;

    private:
        struct CommandPoolBuilderImpl;

        CommandPoolBuilderImpl* _impl;
    };

    class ICommandPool
    {
    protected:
        ICommandPool() = default;

    public:
        virtual ~ICommandPool() = default;

        [[nodiscard]] virtual ICommandBuffer* allocate(
            const ECommandBufferLevel level = ECommandBufferLevel::PRIMARY) = 0;
        [[nodiscard]] virtual vector<ICommandBuffer*> allocate(
            const u32 count,
            const ECommandBufferLevel level = ECommandBufferLevel::PRIMARY) = 0;

        ICommandPool(const ICommandPool&) = delete;
        ICommandPool(ICommandPool&&) noexcept = delete;
        ICommandPool& operator=(const ICommandPool&) = delete;
        ICommandPool& operator=(ICommandPool&&) noexcept = delete;
    };

    class ICommandBuffer
    {
    protected:
        ICommandBuffer() = default;

    public:
        struct RenderPassRecordInfo
        {
            IRenderPass* renderpass;
            IFramebuffer* renderTarget;
            i32 x;
            i32 y;
            u32 width;
            u32 height;
            vector<ClearValue> clearValues;
        };

        virtual ~ICommandBuffer() = default;

        virtual void record() const = 0;
        virtual void end() const = 0;
        virtual void beginRenderPass(const RenderPassRecordInfo& info,
                                     const bool isInline) = 0;
        virtual void endRenderPass() = 0;
        virtual void bind(const IGraphicsPipeline* pipeline) = 0;
        virtual void draw(const u32 vertices, const u32 instances,
                          const u32 baseVertex, const u32 baseInstance) = 0;

        ICommandBuffer(const ICommandBuffer&) = delete;
        ICommandBuffer(ICommandBuffer&&) noexcept = delete;
        ICommandBuffer& operator=(const ICommandBuffer&) = delete;
        ICommandBuffer& operator=(ICommandBuffer&&) noexcept = delete;
    };

    class SemaphoreBuilder
    {
    public:
        SemaphoreBuilder();
        ~SemaphoreBuilder();

        SemaphoreBuilder& device(const IDevice* device);
        [[nodiscard]] ISemaphore* build() const;

        SemaphoreBuilder(const SemaphoreBuilder&) = delete;
        SemaphoreBuilder(SemaphoreBuilder&&) noexcept = delete;
        SemaphoreBuilder& operator=(const SemaphoreBuilder&) = delete;
        SemaphoreBuilder& operator=(SemaphoreBuilder&&) noexcept = delete;

    private:
        struct SemaphoreBuilderImpl;
        SemaphoreBuilderImpl* _impl;
    };

    class ISemaphore
    {
    protected:
        ISemaphore() = default;

    public:
        virtual ~ISemaphore() = default;

        ISemaphore(const ISemaphore&) = delete;
        ISemaphore(ISemaphore&&) noexcept = delete;
        ISemaphore& operator=(const ISemaphore&) = delete;
        ISemaphore& operator=(ISemaphore&&) noexcept = delete;
    };

    class FenceBuilder
    {
    public:
        FenceBuilder();
        ~FenceBuilder();

        FenceBuilder& device(const IDevice* device);
        FenceBuilder& signaled();
        [[nodiscard]] IFence* build() const;

        FenceBuilder(const FenceBuilder&) = delete;
        FenceBuilder(FenceBuilder&&) noexcept = delete;
        FenceBuilder& operator=(const FenceBuilder&) = delete;
        FenceBuilder& operator=(FenceBuilder&&) noexcept = delete;

    private:
        struct FenceBuilderImpl;
        FenceBuilderImpl* _impl;
    };

    class IFence
    {
    protected:
        IFence() = default;

    public:
        virtual ~IFence() = default;

        virtual void reset() = 0;
        virtual void wait() = 0;
        virtual bool status() = 0;

        IFence(const IFence&) = delete;
        IFence(IFence&&) noexcept = delete;
        IFence& operator=(const IFence&) = delete;
        IFence& operator=(IFence&&) noexcept = delete;
    };
} // namespace helios