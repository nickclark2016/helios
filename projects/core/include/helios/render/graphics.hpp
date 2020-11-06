#pragma once

#include <helios/containers/optional.hpp>
#include <helios/containers/utility.hpp>
#include <helios/containers/vector.hpp>
#include <helios/core/window.hpp>
#include <helios/macros.hpp>
#include <helios/render/enums.hpp>

#include <functional>
#include <string>
#include <variant>

namespace helios
{
    union ClearColorValue
    {
        f32 float32[4];
        i32 int32[4];
        u32 uint32[4];
    };

    struct ClearDepthStencilValue
    {
        f32 depth;
        u32 stencil;
    };

    union ClearValue
    {
        ClearColorValue color;
        ClearDepthStencilValue depthStencil;
    };

    class IBuffer;
    class ICommandBuffer;
    class ICommandPool;
    class IContext;
    class IDescriptorPool;
    class IDescriptorSet;
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
    class ISampler;
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

    struct BufferCopyRegion
    {
        u64 srcOffset;
        u64 dstOffset;
        u64 size;
    };

    struct BufferImageCopyRegion
    {
        u64 offset;
        u32 imageWidth;
        u32 imageHeight;
        EImageAspectFlags aspect;
        u32 mipLevel;
        u32 arrayLayer;
        u32 layerCount;
        i32 x;
        i32 y;
        i32 z;
        u32 width;
        u32 height;
        u32 depth;
    };

    struct DescriptorImageInfo
    {
        ISampler* sampler;
        IImageView* view;
        EImageLayout layout;
    };

    struct DescriptorBufferInfo
    {
        IBuffer* buffer;
        u64 offset;
        u64 range;
    };

    struct DescriptorWriteInfo final
    {
        u32 binding;
        u32 element;
        EDescriptorType type;
        std::variant<vector<DescriptorImageInfo>, vector<DescriptorBufferInfo>>
            descriptorInfos;
    };

    struct ImageMemoryBarrier
    {
        EAccessFlags srcAccess;
        EAccessFlags dstAccess;
        EImageLayout oldLayout;
        EImageLayout newLayout;
        u32 srcQueueFamilyIndex;
        u32 dstQueueFamilyIndex;
        IImage* image;
        EImageAspectFlags aspect;
        u32 mipLevel;
        u32 mipCount;
        u32 arrayLayer;
        u32 layerCount;
    };

    struct BufferMemoryBarrier
    {
        EAccessFlags srcAccess;
        EAccessFlags dstAccess;
        u32 srcQueueFamilyIndex;
        u32 dstQueueFamilyIndex;
        IBuffer* buffer;
        u64 offset;
        u64 size;
    };

    class ContextBuilder final
    {
    public:
        ContextBuilder();
        ~ContextBuilder();

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

        HELIOS_NO_COPY_MOVE(ContextBuilder)

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

        [[nodiscard]] virtual vector<IPhysicalDevice*> physicalDevices() = 0;
        virtual void dispose() = 0;

        HELIOS_NO_COPY_MOVE(IContext)
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

        HELIOS_NO_COPY_MOVE(IPhysicalDevice)
    };

    class DeviceBuilder
    {
    public:
        DeviceBuilder();
        ~DeviceBuilder();

        DeviceBuilder& queues(const vector<QueueFamilyInfo>& infos);
        DeviceBuilder& features(const IPhysicalDevice::Features& features);
        DeviceBuilder& validation();
        DeviceBuilder& swapchain();
        DeviceBuilder& physical(const IPhysicalDevice* pPhysical);
        DeviceBuilder& compute(const u32 count);
        DeviceBuilder& transfer(const u32 count);
        DeviceBuilder& graphics(const u32 count);
        [[nodiscard]] IDevice* build() const;

        HELIOS_NO_COPY_MOVE(DeviceBuilder)

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

        virtual ~IQueue() = default;

        [[nodiscard]] virtual float priority() const = 0;
        [[nodiscard]] virtual u32 index() = 0;
        [[nodiscard]] virtual IPhysicalDevice::QueueProperties props()
            const = 0;
        [[nodiscard]] virtual bool canPresent(
            const IPhysicalDevice* device, const ISurface* surface) const = 0;
        virtual void submit(const vector<SubmitInfo>& submitInfo,
                            const IFence* signal) const = 0;
        virtual void present(const PresentInfo& presentInfo) const = 0;

        HELIOS_NO_COPY_MOVE(IQueue)
    };

    class IDevice
    {
    protected:
        IDevice() = default;

    public:
        virtual ~IDevice() = default;

        [[nodiscard]] virtual vector<IQueue*> queues() const = 0;
        virtual void releaseResources() = 0;
        virtual void idle() const = 0;

        HELIOS_NO_COPY_MOVE(IDevice)
    };

    class SurfaceBuilder
    {
    public:
        SurfaceBuilder();
        ~SurfaceBuilder();

        SurfaceBuilder& device(const IDevice* device);
        SurfaceBuilder& window(const IWindow* window);
        [[nodiscard]] ISurface* build() const;

        HELIOS_NO_COPY_MOVE(SurfaceBuilder)

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

        HELIOS_NO_COPY_MOVE(ISurface)
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

        HELIOS_NO_COPY_MOVE(SwapchainBuilder)

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

        HELIOS_NO_COPY_MOVE(ISwapchain)
    };

    class ImageBuilder
    {
    public:
        ImageBuilder();
        ~ImageBuilder();

        ImageBuilder& device(const IDevice* device);
        ImageBuilder& type(const EImageType type);
        ImageBuilder& format(const EFormat format);
        ImageBuilder& extent(const u32 width, const u32 height,
                             const u32 depth);
        ImageBuilder& mipLevels(const u32 levels);
        ImageBuilder& arrayLayers(const u32 layers);
        ImageBuilder& samples(const ESampleCountFlagBits samples);
        ImageBuilder& tiling(const EImageTiling tiling);
        ImageBuilder& usage(const EImageUsageFlags usage);
        ImageBuilder& queues(const vector<IQueue*>& queues);
        ImageBuilder& initialLayout(const EImageLayout layout);
        ImageBuilder& preferredFlags(const EMemoryPropertyFlags flags);
        ImageBuilder& requiredFlags(const EMemoryPropertyFlags flags);
        ImageBuilder& memoryUsage(const EMemoryUsage usage);
        IImage* build() const;

        HELIOS_NO_COPY_MOVE(ImageBuilder)

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

        virtual EFormat getFormat() const noexcept = 0;

        HELIOS_NO_COPY_MOVE(IImage)
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

        HELIOS_NO_COPY_MOVE(ImageViewBuilder)

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

        HELIOS_NO_COPY_MOVE(IImageView)
    };

    class ShaderModuleBuilder
    {
    public:
        ShaderModuleBuilder();
        ~ShaderModuleBuilder();

        ShaderModuleBuilder& device(const IDevice* device);
        ShaderModuleBuilder& source(const vector<u8>& source);
        [[nodiscard]] IShaderModule* build() const;

        HELIOS_NO_COPY_MOVE(ShaderModuleBuilder)

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

        HELIOS_NO_COPY_MOVE(IShaderModule)
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

        HELIOS_NO_COPY_MOVE(DescriptorSetLayoutBuilder)

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

        HELIOS_NO_COPY_MOVE(IDescriptorSetLayout)
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

        HELIOS_NO_COPY_MOVE(PipelineLayoutBuilder)

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

        HELIOS_NO_COPY_MOVE(IPipelineLayout)
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

        HELIOS_NO_COPY_MOVE(GraphicsPipelineBuilder)

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

        HELIOS_NO_COPY_MOVE(IGraphicsPipeline)
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

        HELIOS_NO_COPY_MOVE(RenderPassBuilder)

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

        HELIOS_NO_COPY_MOVE(IRenderPass)
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

        HELIOS_NO_COPY_MOVE(FramebufferBuilder)

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

        HELIOS_NO_COPY_MOVE(IFramebuffer)
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

        HELIOS_NO_COPY_MOVE(CommandPoolBuilder)

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

        HELIOS_NO_COPY_MOVE(ICommandPool)
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
        virtual void draw(const u32 vertices, const u32 instances,
                          const u32 baseVertex, const u32 baseInstance) = 0;
        virtual void draw(const u32 indexCount, const u32 instances,
                          const u32 baseIndex, const i32 vertexOffset,
                          const u32 baseInstance) = 0;
        virtual void bind(const IGraphicsPipeline* pipeline) = 0;
        virtual void bind(const vector<IBuffer*>& buffers,
                          const vector<u64>& offsets, u32 first = 0) = 0;
        virtual void bind(const vector<IDescriptorSet*> descriptorSets,
                          const IGraphicsPipeline* pipeline, u32 first) = 0;
        virtual void bind(IBuffer* elements, u64 offset) = 0;
        virtual void copy(IBuffer* src, IBuffer* dst,
                          const vector<BufferCopyRegion>& regions) = 0;
        virtual void copy(IBuffer* src, IImage* dst,
                          const vector<BufferImageCopyRegion>& regions,
                          const EImageLayout format) = 0;
        virtual void barrier(
            EPipelineStageFlags src, EPipelineStageFlags dst,
            EDependencyFlags dependency,
            const vector<BufferMemoryBarrier>& bufferBarriers, const vector<ImageMemoryBarrier>& imageBarriers) = 0;

        HELIOS_NO_COPY_MOVE(ICommandBuffer)
    };

    class SemaphoreBuilder
    {
    public:
        SemaphoreBuilder();
        ~SemaphoreBuilder();

        SemaphoreBuilder& device(const IDevice* device);
        SemaphoreBuilder& timeline();
        [[nodiscard]] ISemaphore* build() const;

        HELIOS_NO_COPY_MOVE(SemaphoreBuilder)

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

        HELIOS_NO_COPY_MOVE(ISemaphore)
    };

    class FenceBuilder
    {
    public:
        FenceBuilder();
        ~FenceBuilder();

        FenceBuilder& device(const IDevice* device);
        FenceBuilder& signaled();
        [[nodiscard]] IFence* build() const;

        HELIOS_NO_COPY_MOVE(FenceBuilder)

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

        HELIOS_NO_COPY_MOVE(IFence)
    };

    class BufferBuilder
    {
    public:
        BufferBuilder();
        ~BufferBuilder();

        BufferBuilder& device(const IDevice* device);
        BufferBuilder& size(u64 bytes);
        BufferBuilder& queues(const vector<IQueue*>& concurrentAccessQueues);
        BufferBuilder& usage(const EBufferTypeFlags usage);
        BufferBuilder& preferredFlags(const EMemoryPropertyFlags flags);
        BufferBuilder& requiredFlags(const EMemoryPropertyFlags flags);
        BufferBuilder& memoryUsage(const EMemoryUsage usage);
        IBuffer* build() const;

        HELIOS_NO_COPY_MOVE(BufferBuilder)

    private:
        struct BufferBuilderImpl;

        BufferBuilderImpl* _impl;
    };

    class IBuffer
    {
    protected:
        IBuffer() = default;

    public:
        virtual ~IBuffer() = default;

        virtual void* map() = 0;
        virtual void unmap() = 0;

        HELIOS_NO_COPY_MOVE(IBuffer)
    };

    class DescriptorPoolBuilder
    {
    public:
        DescriptorPoolBuilder();
        ~DescriptorPoolBuilder();

        DescriptorPoolBuilder& device(const IDevice* device);
        DescriptorPoolBuilder& maxSetCount(u32 sets);
        DescriptorPoolBuilder& samplers(u32 count);
        DescriptorPoolBuilder& imageSamplers(u32 count);
        DescriptorPoolBuilder& sampledImages(u32 count);
        DescriptorPoolBuilder& storageImages(u32 count);
        DescriptorPoolBuilder& uniformTexelBuffers(u32 count);
        DescriptorPoolBuilder& storageTexelBuffers(u32 count);
        DescriptorPoolBuilder& uniformBuffers(u32 count);
        DescriptorPoolBuilder& storageBuffers(u32 count);
        DescriptorPoolBuilder& dynamicUniformBuffers(u32 count);
        DescriptorPoolBuilder& dynamicStorageBuffers(u32 count);
        DescriptorPoolBuilder& inputAttachments(u32 count);
        IDescriptorPool* build() const;

        HELIOS_NO_COPY_MOVE(DescriptorPoolBuilder)
    private:
        struct DescriptorPoolBuilderImpl;

        DescriptorPoolBuilderImpl* _impl;
    };

    class IDescriptorPool
    {
    protected:
        IDescriptorPool() = default;

    public:
        virtual ~IDescriptorPool() = default;
        virtual vector<IDescriptorSet*> allocate(
            const vector<IDescriptorSetLayout*>& layouts) = 0;
        virtual void reset() = 0;

        virtual u32 maxSets() const = 0;
        virtual u32 allocatedSets() const = 0;
        virtual u32 maxSamplers() const = 0;
        virtual u32 allocatedSamplers() const = 0;
        virtual u32 maxCombinedImageSamplers() const = 0;
        virtual u32 allocatedCombinedImageSamplers() const = 0;
        virtual u32 maxSampledImages() const = 0;
        virtual u32 allocatedSampledImages() const = 0;
        virtual u32 maxStorageImages() const = 0;
        virtual u32 allocatedStorageImages() const = 0;
        virtual u32 maxUniformTexelBuffers() const = 0;
        virtual u32 allocatedUniformTexelBuffers() const = 0;
        virtual u32 maxStorageTexelBuffers() const = 0;
        virtual u32 allocatedStorageTexelBuffers() const = 0;
        virtual u32 maxUniformBuffers() const = 0;
        virtual u32 allocatedUniformBuffers() const = 0;
        virtual u32 maxStorageBuffers() const = 0;
        virtual u32 allocatedStorageBuffers() const = 0;
        virtual u32 maxDynamicUniformBuffers() const = 0;
        virtual u32 allocatedDynamicUniformBuffers() const = 0;
        virtual u32 maxDynamicStorageBuffers() const = 0;
        virtual u32 allocatedDynamicStorageBuffers() const = 0;
        virtual u32 maxInputAttachments() const = 0;
        virtual u32 allocatedInputAttachments() const = 0;

        HELIOS_NO_COPY_MOVE(IDescriptorPool)
    };

    class IDescriptorSet
    {
    protected:
        IDescriptorSet() = default;

    public:
        virtual ~IDescriptorSet() = default;

        virtual void write(const vector<DescriptorWriteInfo>& descriptors) = 0;

        HELIOS_NO_COPY_MOVE(IDescriptorSet)
    };

    class SamplerBuilder
    {
    public:
        SamplerBuilder();
        ~SamplerBuilder();

        SamplerBuilder& device(const IDevice* device);
        SamplerBuilder& magnification(const EFilter filter);
        SamplerBuilder& minification(const EFilter filter);
        SamplerBuilder& mipmap(const ESamplerMipMapMode mode);
        SamplerBuilder& addressModeU(const ESamplerAddressMode mode);
        SamplerBuilder& addressModeV(const ESamplerAddressMode mode);
        SamplerBuilder& addressModeW(const ESamplerAddressMode mode);
        SamplerBuilder& mipLodBias(const f32 bias);
        SamplerBuilder& anisotropy(const f32 max);
        SamplerBuilder& compare(const ECompareOp op);
        SamplerBuilder& minLod(const f32 lod);
        SamplerBuilder& maxLod(const f32 lod);
        SamplerBuilder& border(const EBorderColor color);
        SamplerBuilder& unnormalized(const bool unnormalized);
        ISampler* build() const;

        HELIOS_NO_COPY_MOVE(SamplerBuilder)
    private:
        struct SamplerBuilderImpl;

        SamplerBuilderImpl* _impl;
    };

    class ISampler
    {
    protected:
        ISampler() = default;

    public:
        virtual ~ISampler() = default;

        HELIOS_NO_COPY_MOVE(ISampler)
    };
} // namespace helios