#include <helios/graphics.hpp>

#include <helios/window.hpp>

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <utility>

#define HELIOS_VERSION_MAJOR 0
#define HELIOS_VERSION_MINOR 0
#define HELIOS_VERSION_PATCH 1

namespace helios
{
    template <typename Output, typename Input> Output cast(Input ptr)
    {
        static_assert(std::is_pointer_v<Input> && std::is_pointer_v<Output>,
                      "Input and Output must both be pointers.");
#if defined(_DEBUG)
        return dynamic_cast<Output>(ptr);
#else
        return reinterpret_cast<Output>(ptr);
#endif
    }

    static const std::string shader_entry = "main";
    static const std::string validation_layer = "VK_LAYER_KHRONOS_validation";
    static const std::string debug_extension =
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    static const std::string swapchain_extension =
        VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    extern GLFWwindow* as_native(const IWindow* window);

    static IPhysicalDevice::QueueProperties find_best_transfer(
        const vector<IPhysicalDevice::QueueProperties>& props)
    {
        // first search for dedicated
        for (const auto& fam : props)
        {
            if (fam.transfer && !fam.compute && !fam.graphics)
            {
                return fam;
            }
        }

        // next search for shared with one other
        for (const auto& fam : props)
        {
            if (fam.transfer && fam.compute && !fam.graphics)
            {
                return fam;
            }

            if (fam.transfer && !fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for generic with transfer
        for (const auto& fam : props)
        {
            if (fam.transfer)
            {
                return fam;
            }
        }

        return {~0U, false, false, false, false, false, 0};
    }

    static IPhysicalDevice::QueueProperties find_best_compute(
        const vector<IPhysicalDevice::QueueProperties>& props)
    {
        // first search for dedicated
        for (const auto& fam : props)
        {
            if (!fam.transfer && fam.compute && !fam.graphics)
            {
                return fam;
            }
        }

        // next search for shared with one other
        for (const auto& fam : props)
        {
            if (fam.transfer && fam.compute && !fam.graphics)
            {
                return fam;
            }

            if (!fam.transfer && fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for generic with compute
        for (const auto& fam : props)
        {
            if (fam.compute)
            {
                return fam;
            }
        }

        return {~0U, false, false, false, false, false, 0};
    }

    static IPhysicalDevice::QueueProperties find_best_graphics(
        const vector<IPhysicalDevice::QueueProperties>& props)
    {
        // first search for dedicated
        for (const auto& fam : props)
        {
            if (!fam.transfer && !fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for shared with one other
        for (const auto& fam : props)
        {
            if (!fam.transfer && fam.compute && fam.graphics)
            {
                return fam;
            }

            if (fam.transfer && !fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for generic with graphics
        for (const auto& fam : props)
        {
            if (fam.graphics)
            {
                return fam;
            }
        }

        return {~0U, false, false, false, false, false, 0};
    }

    std::function<void(EMessageSeverity, const std::string)>
        ContextBuilder::DefaultCallback =
            [](EMessageSeverity severity, const std::string& message) {
                if (severity == EMessageSeverity::WARNING ||
                    severity == EMessageSeverity::ERROR)
                {
                    fprintf(stderr, "%s\n", message.c_str());
                }
            };

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  const VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData)
    {

        std::function<void(EMessageSeverity, const char*)>& func =
            *static_cast<std::function<void(EMessageSeverity, const char*)>*>(
                pUserData);

        EMessageSeverity severity = EMessageSeverity::ERROR;
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severity = EMessageSeverity::VERBOSE;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severity = EMessageSeverity::INFO;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severity = EMessageSeverity::WARNING;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severity = EMessageSeverity::ERROR;
            break;
        default:
            break;
        }

        func(severity, pCallbackData->pMessage);

        return VK_FALSE;
    }

    struct ContextBuilder::ContextBuilderImpl
    {
        std::string appName;
        uint32_t appVersion = VK_MAKE_VERSION(0, 0, 0);
        std::string engineName = "Helios";
        uint32_t engineVersion = VK_MAKE_VERSION(
            HELIOS_VERSION_MAJOR, HELIOS_VERSION_MINOR, HELIOS_VERSION_PATCH);
        vector<std::string> extensions;
        vector<std::string> layers;
        std::function<void(EMessageSeverity, const char*)> debugCallback;
    };

    struct DeviceBuilder::DeviceBuilderImpl
    {
        vector<QueueFamilyInfo> queues;
        vector<std::string> extensions;
        vector<std::string> layers;
        IPhysicalDevice::Features features = {};
        bool featuresSet = false;
        IPhysicalDevice* physicalDevice = nullptr;
        uint32_t computeCount = ~0U;
        uint32_t transferCount = ~0U;
        uint32_t graphicsCount = ~0U;
    };

    struct SurfaceBuilder::SurfaceBuilderImpl
    {
        IWindow* window = nullptr;
        IDevice* device = nullptr;
    };

    struct SwapchainBuilder::SwapchainBuilderImpl
    {
        ISurface* surface = nullptr;
        uint32_t count = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t layers = 0;
        ISwapchain* previous = nullptr;
        EPresentMode presentMode = EPresentMode::IMMEDIATE;
        EFormat format = EFormat::UNDEFINED;
        EColorSpace colorSpace = EColorSpace::SRGB_NONLINEAR;
        vector<IQueue*> queues;
        EImageUsageFlags usage = 0;
        ESurfaceTransformFlagBits transform = TRANSFORM_IDENTITY;
        uint32_t alpha = 0;
        bool clipped = false;
    };

    struct ImageBuilder::ImageBuilderImpl
    {
    };

    struct ImageViewBuilder::ImageViewBuilderImpl
    {
        EImageViewType type = EImageViewType::TYPE_2D;
        IImage* image = nullptr;
        EFormat format = EFormat::UNDEFINED;
        EComponentSwizzle red = EComponentSwizzle::IDENTITY;
        EComponentSwizzle green = EComponentSwizzle::IDENTITY;
        EComponentSwizzle blue = EComponentSwizzle::IDENTITY;
        EComponentSwizzle alpha = EComponentSwizzle::IDENTITY;
        EImageAspectFlags aspect = 0;
        uint32_t baseMip = 0;
        uint32_t mipLevels = 1;
        uint32_t baseArrayLayer = 0;
        uint32_t arrayLayers = 1;
    };

    struct DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilderImpl
    {
        vector<DescriptorSetLayoutBinding> bindings;
        IDevice* device = nullptr;
    };

    struct ShaderModuleBuilder::ShaderModuleBuilderImpl
    {
        vector<uint8_t> data;
        IDevice* device = nullptr;
    };

    struct PipelineLayoutBuilder::PipelineLayoutBuilderImpl
    {
        IDevice* device = nullptr;
        vector<PushConstantRange> ranges;
        vector<IDescriptorSetLayout*> layouts;
    };

    struct GraphicsPipelineBuilder::GraphicsPipelineBuilderImpl
    {
        IGraphicsPipeline* parent = nullptr;
        IShaderModule* vertex = nullptr;
        IShaderModule* fragment = nullptr;
        VertexInputState bindings;
        InputAssemblyState input = {};
        TessellationState tessellation = {1};
        ViewportState viewports;
        RasterizationState rasterization;
        MultisampleState multisample;
        DepthStencilState depth;
        ColorBlendState colors;
        DynamicState dynamic;
        IPipelineLayout* layout = nullptr;
        uint32_t subpass = 0;
        IRenderPass* renderpass = nullptr;
        bool allowDerivatives = false;
    };

    struct RenderPassBuilder::RenderPassBuilderImpl
    {
        IDevice* device = nullptr;
        vector<AttachmentDescription> attachments;
        vector<SubpassDescription> subpasses;
        vector<SubpassDependency> dependencies;
    };

    struct FramebufferBuilder::FramebufferBuilderImpl
    {
        vector<IImageView*> views;
        IRenderPass* renderpass = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t layers = 1;
    };

    struct CommandPoolBuilder::CommandPoolBuilderImpl
    {
        IDevice* device = nullptr;
        IQueue* queue = nullptr;
        bool transient = false;
        bool reset = false;
    };

    struct SemaphoreBuilder::SemaphoreBuilderImpl
    {
        IDevice* device = nullptr;
    };

    struct FenceBuilder::FenceBuilderImpl
    {
        IDevice* device = nullptr;
        bool signalled = false;
    };

    struct VulkanCommandBuffer;
    struct VulkanCommandPool;
    struct VulkanDescriptorSetLayout;
    struct VulkanDevice;
    struct VulkanFence;
    struct VulkanFramebuffer;
    struct VulkanGraphicsPipeline;
    struct VulkanImage;
    struct VulkanImageView;
    struct VulkanPhysicalDevice;
    struct VulkanPipelineLayout;
    struct VulkanRenderPass;
    struct VulkanSemaphore;
    struct VulkanShaderModule;
    struct VulkanSurface;
    struct VulkanSwapchain;

    struct VulkanContext final : IContext
    {
        VulkanContext() = default;
        ~VulkanContext();

        [[nodiscard]] vector<IPhysicalDevice*> physicalDevices() const override;

        VulkanContext(const VulkanContext&) = delete;
        VulkanContext(VulkanContext&&) noexcept = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;
        VulkanContext& operator=(VulkanContext&&) noexcept = delete;

        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        std::function<void(EMessageSeverity, const char*)> debugCallback;

        bool destroyed = false;
        vector<VulkanPhysicalDevice*> devices;
    };

    struct VulkanPhysicalDevice final : IPhysicalDevice
    {
        VulkanPhysicalDevice() = default;
        ~VulkanPhysicalDevice();

        [[nodiscard]] std::string name() const override;
        [[nodiscard]] EPhysicalDeviceType type() const override;
        [[nodiscard]] Features features() const override;
        [[nodiscard]] vector<QueueProperties> queueProperties() override;

        VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
        VulkanPhysicalDevice(VulkanPhysicalDevice&&) noexcept = delete;
        VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;
        VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) noexcept =
            delete;

        bool destroyed = false;
        VkPhysicalDevice device = VK_NULL_HANDLE;
        std::string deviceName = "";
        EPhysicalDeviceType deviceType = EPhysicalDeviceType::OTHER;
        VulkanContext* context = nullptr;
        VulkanDevice* logicalDevice = VK_NULL_HANDLE;
    };

    struct VulkanDevice final : IDevice
    {
        VulkanDevice() = default;
        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice(VulkanDevice&&) noexcept = delete;
        ~VulkanDevice();
        VulkanDevice& operator=(const VulkanDevice&) = delete;
        VulkanDevice& operator=(VulkanDevice&&) noexcept = delete;

        [[nodiscard]] vector<IQueue*> queues() const override;
        void idle() const override;

        bool destroyed = false;
        VkDevice device = VK_NULL_HANDLE;
        vector<IQueue*> deviceQueues;
        vector<VulkanCommandPool*> commandBufferPools;
        vector<VulkanDescriptorSetLayout*> setLayouts;
        vector<VulkanFence*> fences;
        vector<VulkanGraphicsPipeline*> graphicsPipelines;
        vector<VulkanImage*> images;
        vector<VulkanPipelineLayout*> pipelineLayouts;
        vector<VulkanRenderPass*> renderPasses;
        vector<VulkanSemaphore*> sems;
        vector<VulkanShaderModule*> modules;
        vector<VulkanSurface*> surfaces;
        VulkanPhysicalDevice* parent = nullptr;
    };

    struct VulkanQueue final : IQueue
    {
        VulkanQueue() = default;
        ~VulkanQueue() override = default;
        [[nodiscard]] float priority() const override;
        [[nodiscard]] uint32_t index() override;
        [[nodiscard]] IPhysicalDevice::QueueProperties props() const override;
        [[nodiscard]] bool canPresent(const IPhysicalDevice* device,
                                      const ISurface* surface) const override;
        void submit(const vector<SubmitInfo>& submitInfo,
                    const IFence* fence) const override;
        void present(const PresentInfo& presentInfo) const override;

        bool destroyed = false;
        IPhysicalDevice::QueueProperties family = {};
        uint32_t queueIndex = 0;
        float queuePriority = 0.0f;
        VkQueue queue = VK_NULL_HANDLE;

        VulkanQueue(const VulkanQueue&) = delete;
        VulkanQueue(VulkanQueue&&) noexcept = delete;
        VulkanQueue& operator=(const VulkanQueue&) = delete;
        VulkanQueue& operator=(VulkanQueue&&) noexcept = delete;
    };

    struct VulkanSurface final : ISurface
    {
        VulkanSurface() = default;
        ~VulkanSurface() override;
        [[nodiscard]] IWindow* window() const override;
        [[nodiscard]] SwapchainSupport swapchainSupport(
            const IPhysicalDevice* device) const override;

        VkSurfaceKHR surface = nullptr;

        VulkanContext* context = nullptr;
        VulkanDevice* device = nullptr;
        IWindow* windowPtr = nullptr;
        VulkanSwapchain* swapchain = nullptr;
        bool destroyed = false;

        VulkanSurface(const VulkanSurface&) = delete;
        VulkanSurface(VulkanSurface&&) noexcept = delete;
        VulkanSurface& operator=(const VulkanSurface&) = delete;
        VulkanSurface& operator=(VulkanSurface&&) noexcept = delete;
    };

    struct VulkanSwapchain final : ISwapchain
    {
        VulkanSwapchain() = default;
        ~VulkanSwapchain() override;
        [[nodiscard]] uint32_t imagesCount() const override;
        [[nodiscard]] vector<IImageView*> views() const override;
        [[nodiscard]] EFormat format() const override;
        [[nodiscard]] uint32_t acquireNextImage(const uint64_t wait,
                                                const ISemaphore* signal,
                                                const IFence* fence) override;

        bool destroyed = false;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanSurface* surface = nullptr;
        vector<IImageView*> imgViews;
        EFormat fmt = EFormat::UNDEFINED;

        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain(VulkanSwapchain&&) noexcept = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(VulkanSwapchain&&) noexcept = delete;
    };

    struct VulkanImage final : IImage
    {
        VulkanImage() = default;
        ~VulkanImage() override;

        bool destroyed = false;
        VkImage image = VK_NULL_HANDLE;
        bool owned = false;
        VulkanDevice* device = nullptr;

        vector<VulkanImageView*> views;

        VulkanImage(const VulkanImage&) = delete;
        VulkanImage(VulkanImage&&) noexcept = delete;
        VulkanImage& operator=(const VulkanImage&) = delete;
        VulkanImage& operator=(VulkanImage&&) noexcept = delete;
    };

    struct VulkanImageView final : IImageView
    {
        VulkanImageView() = default;
        ~VulkanImageView() override;

        bool destroyed = false;
        VkImageView view = VK_NULL_HANDLE;
        VulkanImage* image = nullptr;

        VulkanImageView(const VulkanImageView&) = delete;
        VulkanImageView(VulkanImageView&&) noexcept = delete;
        VulkanImageView& operator=(const VulkanImageView) = delete;
        VulkanImageView& operator=(VulkanImageView&&) noexcept = delete;
    };

    struct VulkanShaderModule final : IShaderModule
    {
        VulkanShaderModule() = default;
        ~VulkanShaderModule() override;

        bool destroyed = false;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;

        VulkanShaderModule(const VulkanShaderModule&) = delete;
        VulkanShaderModule(VulkanShaderModule&&) noexcept = delete;
        VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
        VulkanShaderModule& operator=(VulkanShaderModule&&) noexcept = delete;
    };

    struct VulkanDescriptorSetLayout final : IDescriptorSetLayout
    {
        VulkanDescriptorSetLayout() = default;
        ~VulkanDescriptorSetLayout() override;
        [[nodiscard]] vector<DescriptorSetLayoutBinding> bindings()
            const override;

        bool destroyed = false;
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        vector<DescriptorSetLayoutBinding> layoutBindings;

        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
        VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&) noexcept =
            delete;
        VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) =
            delete;
        VulkanDescriptorSetLayout& operator=(
            VulkanDescriptorSetLayout&&) noexcept = delete;
    };

    struct VulkanPipelineLayout final : IPipelineLayout
    {
        VulkanPipelineLayout() = default;
        ~VulkanPipelineLayout() override;

        bool destroyed = false;
        VkPipelineLayout layout = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        vector<PushConstantRange> ranges;
        vector<VulkanDescriptorSetLayout*> layouts;

        VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;
        VulkanPipelineLayout(VulkanPipelineLayout&&) noexcept = delete;
        VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;
        VulkanPipelineLayout& operator=(VulkanPipelineLayout&&) noexcept =
            delete;
    };

    struct VulkanRenderPass final : IRenderPass
    {
        VulkanRenderPass() = default;
        ~VulkanRenderPass() override;

        bool destroyed = false;
        VkRenderPass renderpass = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        vector<VulkanFramebuffer*> framebuffers;

        VulkanRenderPass(const VulkanRenderPass&) = delete;
        VulkanRenderPass(VulkanRenderPass&&) noexcept = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
        VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = delete;
    };

    struct VulkanGraphicsPipeline final : IGraphicsPipeline
    {
        VulkanGraphicsPipeline() = default;
        ~VulkanGraphicsPipeline() override;

        bool destroyed = false;
        VkPipeline pipeline = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanPipelineLayout* layout = nullptr;
        VulkanRenderPass* renderpass = nullptr;

        VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
        VulkanGraphicsPipeline(VulkanGraphicsPipeline&&) noexcept = delete;
        VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) =
            delete;
        VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&&) noexcept =
            delete;
    };

    struct VulkanFramebuffer final : IFramebuffer
    {
        VulkanFramebuffer() = default;
        ~VulkanFramebuffer() override;

        [[nodiscard]] uint32_t width() const override;
        [[nodiscard]] uint32_t height() const override;
        [[nodiscard]] uint32_t layers() const override;
        [[nodiscard]] vector<IImageView*> attachments() const override;

        bool destroyed = false;
        VulkanRenderPass* pass = nullptr;
        vector<VulkanImageView*> images;
        VkFramebuffer fb = VK_NULL_HANDLE;
        uint32_t fbWidth = 0;
        uint32_t fbHeight = 0;
        uint32_t fbLayers = 0;

        VulkanFramebuffer(const VulkanFramebuffer&) = delete;
        VulkanFramebuffer(VulkanFramebuffer&&) noexcept = delete;
        VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;
        VulkanFramebuffer& operator=(VulkanFramebuffer&&) noexcept = delete;
    };

    struct VulkanCommandPool final : ICommandPool
    {
        VulkanCommandPool() = default;
        ~VulkanCommandPool() override;

        [[nodiscard]] ICommandBuffer* allocate(
            const ECommandBufferLevel level) override;
        [[nodiscard]] vector<ICommandBuffer*> allocate(
            const uint32_t count, const ECommandBufferLevel level) override;

        bool destroyed = false;
        VkCommandPool pool = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;
        VulkanQueue* queue = nullptr;

        vector<VulkanCommandBuffer*> buffers;

        VulkanCommandPool(const VulkanCommandPool&) = delete;
        VulkanCommandPool(VulkanCommandPool&&) noexcept = delete;
        VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
        VulkanCommandPool& operator=(VulkanCommandPool&&) noexcept = delete;
    };

    struct VulkanCommandBuffer final : ICommandBuffer
    {
        VulkanCommandBuffer() = default;
        ~VulkanCommandBuffer() override;

        void record() const override;
        void end() const override;
        void beginRenderPass(const RenderPassRecordInfo& info,
                             const bool isInline) override;
        void endRenderPass() override;
        void bind(const IGraphicsPipeline* pipeline) override;
        void draw(const uint32_t vertices, const uint32_t instances,
                  const uint32_t baseVertex,
                  const uint32_t baseInstance) override;

        bool destroyed = false;
        VulkanCommandPool* pool = nullptr;
        VkCommandBuffer buffer = VK_NULL_HANDLE;

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept = delete;
    };

    struct VulkanSemaphore final : ISemaphore
    {
        VulkanSemaphore() = default;
        ~VulkanSemaphore() override;

        bool destroyed = false;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;

        VulkanSemaphore(const VulkanSemaphore&) = delete;
        VulkanSemaphore(VulkanSemaphore&&) noexcept = delete;
        VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;
        VulkanSemaphore& operator=(VulkanSemaphore&&) noexcept = delete;
    };

    struct VulkanFence final : IFence
    {
        VulkanFence() = default;
        ~VulkanFence() override;

        void reset() override;
        void wait() override;
        bool status() override;

        bool destroyed = false;
        VkFence fence = VK_NULL_HANDLE;
        VulkanDevice* device = nullptr;

        VulkanFence(const VulkanFence&) = delete;
        VulkanFence(VulkanFence&&) noexcept = delete;
        VulkanFence& operator=(const VulkanFence&) = delete;
        VulkanFence& operator=(VulkanFence&&) noexcept = delete;
    };

    // IMPLEMENTATION

    std::string stringify(const EPhysicalDeviceType& pType)
    {
        switch (pType)
        {
        case EPhysicalDeviceType::OTHER:
            return "OTHER";
        case EPhysicalDeviceType::INTEGRATED:
            return "INTEGRATED";
        case EPhysicalDeviceType::DISCRETE:
            return "DISCRETE";
        case EPhysicalDeviceType::VIRTUAL:
            return "VIRTUAL";
        case EPhysicalDeviceType::CPU:
            return "CPU";
        default:;
        }
        return "UNKNOWN";
    }

    ContextBuilder::ContextBuilder()
    {
        _impl = new ContextBuilderImpl;
    }

    ContextBuilder::~ContextBuilder()
    {
        delete _impl;
    }

    ContextBuilder& ContextBuilder::applicationName(const std::string& name)
    {
        _impl->appName = name;
        return *this;
    }

    ContextBuilder& ContextBuilder::applicationVersion(const uint32_t major,
                                                       const uint32_t minor,
                                                       const uint32_t revision)
    {
        _impl->appVersion = VK_MAKE_VERSION(major, minor, revision);
        return *this;
    }

    ContextBuilder& ContextBuilder::engineName(const std::string& name)
    {
        _impl->engineName = name;
        return *this;
    }

    ContextBuilder& ContextBuilder::engineVersion(const uint32_t major,
                                                  const uint32_t minor,
                                                  const uint32_t revision)
    {
        _impl->engineVersion = VK_MAKE_VERSION(major, minor, revision);
        return *this;
    }

    ContextBuilder& ContextBuilder::validation(
        std::function<void(EMessageSeverity, std::string)> callback)
    {
        _impl->debugCallback = std::move(callback);
        return *this;
    }

    IContext* ContextBuilder::build() const
    {
        static bool init = gladLoaderLoadVulkan(VK_NULL_HANDLE, VK_NULL_HANDLE,
                                                VK_NULL_HANDLE) > 0;

        if (!init)
        {
            exit(-1);
        }

        {
            // get GLFW required extensions
            uint32_t extensionCount;
            const char** extensions =
                glfwGetRequiredInstanceExtensions(&extensionCount);
            for (uint32_t i = 0; i < extensionCount; i++)
            {
                const char* extension = extensions[i];
                _impl->extensions.emplace_back(extension);
            }

            if (_impl->debugCallback)
            {
                _impl->extensions.emplace_back(debug_extension);
                _impl->layers.emplace_back(validation_layer);
            }
        }

        VulkanContext* ctx = new VulkanContext;
        ctx->debugCallback = _impl->debugCallback;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.applicationVersion = _impl->appVersion;
        appInfo.engineVersion = _impl->engineVersion;
        appInfo.pApplicationName = _impl->appName.c_str();
        appInfo.pEngineName = _impl->engineName.c_str();

        vector<const char*> layers;
        vector<const char*> extensions;

        for (const auto& layer : _impl->layers)
        {
            layers.push_back(layer.c_str());
        }

        for (const auto& extension : _impl->extensions)
        {
            extensions.push_back(extension.c_str());
        }

        VkInstanceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.enabledLayerCount = static_cast<uint32_t>(_impl->layers.size());
        info.ppEnabledLayerNames = layers.data();
        info.enabledExtensionCount =
            static_cast<uint32_t>(_impl->extensions.size());
        info.ppEnabledExtensionNames = extensions.data();
        info.pApplicationInfo = &appInfo;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = &ctx->debugCallback;

        if (_impl->debugCallback)
        {
            info.pNext = &debugCreateInfo;
        }

        if (vkCreateInstance(&info, nullptr, &ctx->instance) != VK_SUCCESS)
        {
            delete ctx;
            ctx = nullptr;
        }

        if (ctx && _impl->debugCallback)
        {
            const auto func =
                reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                    vkGetInstanceProcAddr(ctx->instance,
                                          "vkCreateDebugUtilsMessengerEXT"));
            if (func != nullptr)
            {
                func(ctx->instance, &debugCreateInfo, nullptr,
                     &ctx->debugMessenger);
            }
            else
            {
                fprintf(
                    stderr,
                    "Failed to load function vkCreateDebugUtilsMessengerEXT.  "
                    "Extension not present.");
                delete ctx;
                ctx = nullptr;
            }
        }

        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, nullptr);
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, devices.data());

        for (const auto& dev : devices)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);

            VulkanPhysicalDevice* device = new VulkanPhysicalDevice;
            device->device = dev;
            device->deviceName = props.deviceName;
            device->deviceType =
                static_cast<EPhysicalDeviceType>(props.deviceType);
            device->context = ctx;

            ctx->devices.push_back(device);
        }

        return ctx;
    }

    DeviceBuilder::DeviceBuilder()
    {
        _impl = new DeviceBuilderImpl;
    }

    DeviceBuilder::~DeviceBuilder()
    {
        delete _impl;
    }

    DeviceBuilder& DeviceBuilder::queues(const vector<QueueFamilyInfo>& infos)
    {
        _impl->queues = infos;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::features(
        const IPhysicalDevice::Features& features)
    {
        _impl->features = features;
        _impl->featuresSet = true;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::validation()
    {
        _impl->layers.push_back(validation_layer);
        return *this;
    }

    DeviceBuilder& DeviceBuilder::swapchain()
    {
        _impl->extensions.push_back(swapchain_extension);
        return *this;
    }

    DeviceBuilder& DeviceBuilder::physical(const IPhysicalDevice* pPhysical)
    {
        _impl->physicalDevice = const_cast<IPhysicalDevice*>(pPhysical);
        return *this;
    }

    DeviceBuilder& DeviceBuilder::compute(const uint32_t count)
    {
        _impl->computeCount = count;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::transfer(const uint32_t count)
    {
        _impl->transferCount = count;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::graphics(const uint32_t count)
    {
        _impl->graphicsCount = count;
        return *this;
    }

    IDevice* DeviceBuilder::build() const
    {
        VulkanDevice* device = new VulkanDevice;

        IPhysicalDevice* devPtr = _impl->physicalDevice;
        if (VulkanPhysicalDevice* vkDevPtr =
                dynamic_cast<VulkanPhysicalDevice*>(devPtr))
        {
            static bool init =
                gladLoaderLoadVulkan(vkDevPtr->context->instance,
                                     vkDevPtr->device, VK_NULL_HANDLE) > 0;
            if (!init)
            {
                exit(-1);
            }
        }

        VkPhysicalDeviceFeatures features;
        if (!_impl->featuresSet)
        {
            if (VulkanPhysicalDevice* vkDevPtr =
                    cast<VulkanPhysicalDevice*>(devPtr))
            {
                vkGetPhysicalDeviceFeatures(vkDevPtr->device, &features);
            }
        }
        else
        {
            IPhysicalDevice::Features& feats = _impl->features;
            features = {feats.robustBufferAccess,
                        feats.fullDrawUint32,
                        feats.imageCubeArray,
                        feats.independentBlend,
                        feats.geometryShader,
                        feats.tessellationShader,
                        feats.sampleRateShading,
                        feats.dualSourceBlend,
                        feats.logicOperations,
                        feats.multidrawIndirect,
                        feats.drawIndirectFirstInstance,
                        feats.depthClamp,
                        feats.depthBiasClamp,
                        feats.fillModeNonSolid,
                        feats.depthBounds,
                        feats.wideLines,
                        feats.largePoints,
                        feats.alphaToOne,
                        feats.multiViewport,
                        feats.samplerAnisotropy,
                        feats.textureCompressionEtc2,
                        feats.textureCompressionAstcLdr,
                        feats.textureCompressionBc,
                        feats.occlusionQueryPrecise,
                        feats.pipelineStatisticsQuery,
                        feats.vertexPipelineStoresAndAtomics,
                        feats.fragmentStoresAndAtomics,
                        feats.shaderTessellationAndGeometryPointSize,
                        feats.shaderImageGatherExtended,
                        feats.shaderStorageImageExtendedFormats,
                        feats.shaderStorageImageMultisample,
                        feats.shaderStorageImageReadWithoutFormat,
                        feats.shaderStorageImageWriteWithoutFormat,
                        feats.shaderUniformBufferArrayDynamicIndexing,
                        feats.shaderSampledImageArrayDynamicIndexing,
                        feats.shaderStorageBufferArrayDynamicIndexing,
                        feats.shaderStorageImageArrayDynamicIndexing,
                        feats.shaderClipDistance,
                        feats.shaderCullDistance,
                        feats.shaderFloat64,
                        feats.shaderInt64,
                        feats.shaderInt16,
                        feats.shaderResourceResidency,
                        feats.shaderResourceMinLod,
                        feats.sparseBinding,
                        feats.sparseResidencyBuffer,
                        feats.sparseResidencyImage2D,
                        feats.sparseResidencyImage3D,
                        feats.sparseResidency2Samples,
                        feats.sparseResidency4Samples,
                        feats.sparseResidency8Samples,
                        feats.sparseResidency16Samples,
                        feats.sparseResidencyAliased,
                        feats.variableMultisampleRate,
                        feats.inheritedQueries};
        }

        vector<VkDeviceQueueCreateInfo> deviceQueueInfos;
        vector<vector<float>> queuePris;
        deviceQueueInfos.reserve(_impl->queues.size());
        queuePris.reserve(_impl->queues.size());

        if (_impl->queues.empty())
        {
            const auto queues = _impl->physicalDevice->queueProperties();

            const auto transfer = find_best_transfer(queues);
            const auto compute = find_best_compute(queues);
            const auto graphics = find_best_graphics(queues);

            if (_impl->transferCount == ~0U)
            {
                _impl->transferCount = 1;
            }

            if (_impl->computeCount == ~0U)
            {
                _impl->computeCount = 1;
            }

            if (_impl->graphicsCount == ~0U)
            {
                _impl->graphicsCount = 1;
            }

            if (_impl->transferCount > 0)
            {
                _impl->queues.push_back(
                    {transfer.index, false,
                     vector<float>(
                         _impl->transferCount,
                         1.0f / static_cast<float>(_impl->transferCount))});
            }

            if (_impl->computeCount > 0)
            {
                _impl->queues.push_back(
                    {compute.index, false,
                     vector<float>(
                         _impl->computeCount,
                         1.0f / static_cast<float>(_impl->computeCount))});
            }

            if (_impl->graphicsCount > 0)
            {
                _impl->queues.push_back(
                    {graphics.index, false,
                     vector<float>(
                         _impl->graphicsCount,
                         1.0f / static_cast<float>(_impl->graphicsCount))});
            }
        }

        std::sort(_impl->queues.begin(), _impl->queues.end(),
                  [](QueueFamilyInfo& left, QueueFamilyInfo& right) {
                      return left.family < right.family;
                  });

        for (const auto& info : _impl->queues)
        {
            VkDeviceQueueCreateInfo queue = {};
            queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue.queueFamilyIndex = info.family;
            queue.flags |=
                info.protect ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
            queuePris.push_back(info.priorities);
            queue.pQueuePriorities = queuePris[queuePris.size() - 1].data();
            queue.queueCount = static_cast<uint32_t>(info.priorities.size());
            deviceQueueInfos.push_back(queue);
        }

        vector<const char*> layers;
        vector<const char*> extensions;

        for (const auto& layer : _impl->layers)
        {
            layers.push_back(layer.c_str());
        }

        for (const auto& ext : _impl->extensions)
        {
            extensions.push_back(ext.c_str());
        }

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        info.enabledLayerCount = static_cast<uint32_t>(layers.size());
        info.pEnabledFeatures = &features;
        info.ppEnabledExtensionNames = extensions.data();
        info.ppEnabledLayerNames = layers.data();
        info.pQueueCreateInfos = deviceQueueInfos.data();
        info.queueCreateInfoCount =
            static_cast<uint32_t>(deviceQueueInfos.size());

        if (VulkanPhysicalDevice* vkDevPtr =
                dynamic_cast<VulkanPhysicalDevice*>(devPtr))
        {
            if (vkCreateDevice(vkDevPtr->device, &info, nullptr,
                               &device->device) != VK_SUCCESS)
            {
                device = nullptr;
            }

            static bool init =
                gladLoaderLoadVulkan(vkDevPtr->context->instance,
                                     vkDevPtr->device, device->device) > 0;
            if (!init)
            {
                exit(-1);
            }

            const auto queues = _impl->physicalDevice->queueProperties();
            if (!_impl->queues.empty())
            {
                for (const auto& queueInfo : _impl->queues)
                {
                    const auto idx = queueInfo.family;
                    for (const auto prop : queues)
                    {
                        if (prop.index == idx)
                        {
                            for (uint32_t i = 0;
                                 i < static_cast<uint32_t>(
                                         queueInfo.priorities.size());
                                 i++)
                            {
                                VulkanQueue* queue = new VulkanQueue;
                                queue->family = prop;
                                queue->queueIndex = i;
                                queue->queuePriority = queueInfo.priorities[i];
                                vkGetDeviceQueue(
                                    device->device, queue->family.index,
                                    queue->queueIndex, &queue->queue);
                                device->deviceQueues.push_back(queue);
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                // No queues specified, populate all queues
                for (const auto prop : queues)
                {
                    for (uint32_t i = 0; i < prop.count; i++)
                    {
                        VulkanQueue* queue = new VulkanQueue;
                        queue->family = prop;
                        queue->queueIndex = i;
                        queue->queuePriority =
                            1.0f / static_cast<float>(prop.count);
                        vkGetDeviceQueue(device->device, queue->family.index,
                                         queue->queueIndex, &queue->queue);
                        device->deviceQueues.push_back(queue);
                    }
                }
            }
        }

        device->parent = cast<VulkanPhysicalDevice*>(_impl->physicalDevice);
        device->parent->logicalDevice = device;
        return device;
    }

    SurfaceBuilder::SurfaceBuilder()
    {
        _impl = new SurfaceBuilderImpl;
    }

    SurfaceBuilder::~SurfaceBuilder()
    {
        delete _impl;
    }

    SurfaceBuilder& SurfaceBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    SurfaceBuilder& SurfaceBuilder::window(const IWindow* window)
    {
        _impl->window = const_cast<IWindow*>(window);
        return *this;
    }

    ISurface* SurfaceBuilder::build() const
    {
        VulkanSurface* surface = new VulkanSurface;
        surface->device = cast<VulkanDevice*>(_impl->device);
        surface->context =
            cast<VulkanContext*>(surface->device->parent->context);
        surface->device->surfaces.push_back(surface);

        GLFWwindow* win = as_native(_impl->window);

        VkSurfaceKHR surf;
        glfwCreateWindowSurface(surface->context->instance, win, nullptr,
                                &surf);
        surface->surface = surf;

        return surface;
    }

    SwapchainBuilder::SwapchainBuilder()
    {
        _impl = new SwapchainBuilderImpl;
    }

    SwapchainBuilder::~SwapchainBuilder()
    {
        delete _impl;
    }

    SwapchainBuilder& SwapchainBuilder::surface(const ISurface* surface)
    {
        _impl->surface = const_cast<ISurface*>(surface);
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::images(const uint32_t count)
    {
        _impl->count = count;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::width(const uint32_t width)
    {
        _impl->width = width;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::height(const uint32_t height)
    {
        _impl->height = height;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::layers(const uint32_t layers)
    {
        _impl->layers = layers;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::previous(const ISwapchain* previous)
    {
        _impl->previous = const_cast<ISwapchain*>(previous);
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::present(const EPresentMode present)
    {
        _impl->presentMode = present;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::format(const EFormat format)
    {
        _impl->format = format;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::colorSpace(const EColorSpace colorSpace)
    {
        _impl->colorSpace = colorSpace;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::queues(const vector<IQueue*>& queues)
    {
        _impl->queues = queues;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::usage(const EImageUsageFlags usage)
    {
        _impl->usage = usage;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::transform(
        const ESurfaceTransformFlagBits transform)
    {
        _impl->transform = transform;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaOpaque()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaPremultiply()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaPostmultiply()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::alphaInherit()
    {
        _impl->alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        return *this;
    }

    SwapchainBuilder& SwapchainBuilder::clipped()
    {
        _impl->clipped = VK_TRUE;
        return *this;
    }

    ISwapchain* SwapchainBuilder::build() const
    {
        VulkanSwapchain* result = new VulkanSwapchain;

        vector<uint32_t> queueFamilies;
        for (const auto& queue : _impl->queues)
        {
            const uint32_t index = queue->props().index;
            if (std::find(queueFamilies.begin(), queueFamilies.end(), index) ==
                queueFamilies.end())
            {
                queueFamilies.push_back(index);
            }
        }

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.minImageCount = _impl->count;
        info.imageFormat = static_cast<VkFormat>(_impl->format);
        info.imageColorSpace = static_cast<VkColorSpaceKHR>(_impl->colorSpace);
        info.surface = cast<VulkanSurface*>(_impl->surface)->surface;
        info.imageExtent = {_impl->width, _impl->height};
        info.imageArrayLayers = _impl->layers;
        info.imageUsage = _impl->usage;
        info.imageSharingMode = queueFamilies.size() > 1
                                    ? VK_SHARING_MODE_CONCURRENT
                                    : VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount =
            static_cast<uint32_t>(queueFamilies.size());
        info.pQueueFamilyIndices = queueFamilies.data();
        info.preTransform =
            static_cast<VkSurfaceTransformFlagBitsKHR>(_impl->transform);
        info.compositeAlpha =
            static_cast<VkCompositeAlphaFlagBitsKHR>(_impl->alpha);
        info.clipped = _impl->clipped ? VK_TRUE : VK_FALSE;
        info.oldSwapchain =
            _impl->previous ? cast<VulkanSwapchain*>(_impl->previous)->swapchain
                            : VK_NULL_HANDLE;

        VulkanDevice* device =
            cast<VulkanDevice*>(cast<VulkanSurface*>(_impl->surface)->device);

        VkSwapchainKHR swapchain;
        vkCreateSwapchainKHR(device->device, &info, nullptr, &swapchain);
        result->swapchain = swapchain;
        result->device = cast<VulkanDevice*>(device);

        uint32_t imageCount;
        vkGetSwapchainImagesKHR(result->device->device, result->swapchain,
                                &imageCount, nullptr);
        vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(result->device->device, result->swapchain,
                                &imageCount, images.data());

        vector<IImageView*> views;
        for (const auto image : images)
        {
            VulkanImage* img = new VulkanImage;
            img->image = image;
            img->device = result->device;
            img->owned = false;
            device->images.push_back(img);

            const auto view = ImageViewBuilder()
                                  .type(EImageViewType::TYPE_2D)
                                  .image(img)
                                  .aspect(ASPECT_COLOR)
                                  .format(_impl->format)
                                  .build();
            views.push_back(view);
        }
        result->imgViews = views;
        result->fmt = _impl->format;
        result->surface = cast<VulkanSurface*>(_impl->surface);
        result->surface->swapchain = result;

        return result;
    }

    ImageBuilder::ImageBuilder()
    {
        _impl = new ImageBuilderImpl;
    }

    ImageBuilder::~ImageBuilder()
    {
        delete _impl;
    }

    ImageViewBuilder::ImageViewBuilder()
    {
        _impl = new ImageViewBuilderImpl;
    }

    ImageViewBuilder::~ImageViewBuilder()
    {
        delete _impl;
    }

    ImageViewBuilder& ImageViewBuilder::type(const EImageViewType type)
    {
        _impl->type = type;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::image(const IImage* image)
    {
        _impl->image = const_cast<IImage*>(image);
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::format(const EFormat format)
    {
        _impl->format = format;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::redMapping(const EComponentSwizzle red)
    {
        _impl->red = red;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::greenMapping(
        const EComponentSwizzle green)
    {
        _impl->green = green;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::blueMapping(
        const EComponentSwizzle blue)
    {
        _impl->blue = blue;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::alphaMapping(
        const EComponentSwizzle alpha)
    {
        _impl->alpha = alpha;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::aspect(const EImageAspectFlags aspect)
    {
        _impl->aspect = aspect;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::baseMipLevel(const uint32_t base)
    {
        _impl->baseMip = base;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::mipLevels(const uint32_t count)
    {
        _impl->mipLevels = count;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::baseArrayLayer(const uint32_t base)
    {
        _impl->baseArrayLayer = base;
        return *this;
    }

    ImageViewBuilder& ImageViewBuilder::arrayLayers(const uint32_t count)
    {
        _impl->arrayLayers = count;
        return *this;
    }

    IImageView* ImageViewBuilder::build() const
    {
        const auto image = cast<VulkanImage*>(_impl->image);

        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = image->image;
        info.viewType = static_cast<VkImageViewType>(_impl->type);
        info.format = static_cast<VkFormat>(_impl->format);
        info.components = {static_cast<VkComponentSwizzle>(_impl->red),
                           static_cast<VkComponentSwizzle>(_impl->green),
                           static_cast<VkComponentSwizzle>(_impl->blue),
                           static_cast<VkComponentSwizzle>(_impl->alpha)};
        info.subresourceRange = {static_cast<VkImageAspectFlags>(_impl->aspect),
                                 _impl->baseMip, _impl->mipLevels,
                                 _impl->baseArrayLayer, _impl->arrayLayers};

        const auto device = image->device;

        VulkanImageView* view = new VulkanImageView;
        view->image = image;

        vkCreateImageView(image->device->device, &info, nullptr, &view->view);

        image->views.push_back(view);

        return view;
    }

    ShaderModuleBuilder::ShaderModuleBuilder()
    {
        _impl = new ShaderModuleBuilderImpl;
    }

    ShaderModuleBuilder::~ShaderModuleBuilder()
    {
        delete _impl;
    }

    ShaderModuleBuilder& ShaderModuleBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    ShaderModuleBuilder& ShaderModuleBuilder::source(
        const vector<uint8_t>& source)
    {
        _impl->data = source;
        return *this;
    }

    IShaderModule* ShaderModuleBuilder::build() const
    {
        VulkanShaderModule* shader = new VulkanShaderModule;
        shader->device = cast<VulkanDevice*>(_impl->device);

        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = static_cast<uint32_t>(_impl->data.size());
        info.pCode = reinterpret_cast<uint32_t*>(_impl->data.data());
        vkCreateShaderModule(shader->device->device, &info, nullptr,
                             &shader->shaderModule);
        shader->device->modules.push_back(shader);

        return shader;
    }

    DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder()
    {
        _impl = new DescriptorSetLayoutBuilderImpl;
    }

    DescriptorSetLayoutBuilder::~DescriptorSetLayoutBuilder()
    {
        delete _impl;
    }

    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::device(
        const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::bindings(
        const vector<DescriptorSetLayoutBinding>& bindings)
    {
        _impl->bindings = bindings;
        return *this;
    }

    IDescriptorSetLayout* DescriptorSetLayoutBuilder::build() const
    {
        VulkanDescriptorSetLayout* layout = new VulkanDescriptorSetLayout;
        layout->device = cast<VulkanDevice*>(_impl->device);
        layout->layoutBindings = _impl->bindings;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        vector<VkDescriptorSetLayoutBinding> bindings;

        for (const auto& binding : _impl->bindings)
        {
            bindings.push_back({binding.binding,
                                static_cast<VkDescriptorType>(binding.type),
                                binding.count, binding.stages, nullptr});
        }

        info.bindingCount = static_cast<uint32_t>(bindings.size());
        info.pBindings = bindings.data();

        vkCreateDescriptorSetLayout(layout->device->device, &info, nullptr,
                                    &layout->layout);

        return layout;
    }

    PipelineLayoutBuilder::PipelineLayoutBuilder()
    {
        _impl = new PipelineLayoutBuilderImpl;
    }

    PipelineLayoutBuilder::~PipelineLayoutBuilder()
    {
        delete _impl;
    }

    PipelineLayoutBuilder& PipelineLayoutBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    PipelineLayoutBuilder& PipelineLayoutBuilder::layouts(
        const vector<IDescriptorSetLayout*>& layouts)
    {
        _impl->layouts = layouts;
        return *this;
    }

    PipelineLayoutBuilder& PipelineLayoutBuilder::pushConstants(
        const vector<PushConstantRange>& ranges)
    {
        _impl->ranges = ranges;
        return *this;
    }

    IPipelineLayout* PipelineLayoutBuilder::build() const
    {
        VulkanPipelineLayout* pipelineLayout = new VulkanPipelineLayout;
        pipelineLayout->device =
            _impl->layouts.empty()
                ? cast<VulkanDevice*>(_impl->device)
                : cast<VulkanDescriptorSetLayout*>(_impl->layouts[0])->device;

        for (const auto& layout : _impl->layouts)
        {
            pipelineLayout->layouts.push_back(
                cast<VulkanDescriptorSetLayout*>(layout));
        }

        pipelineLayout->ranges = _impl->ranges;

        vector<VkDescriptorSetLayout> layouts;
        vector<VkPushConstantRange> ranges;

        for (const auto range : _impl->ranges)
        {
            ranges.push_back({range.stages, range.offset, range.size});
        }

        for (const auto& layout : _impl->layouts)
        {
            layouts.push_back(cast<VulkanDescriptorSetLayout*>(layout)->layout);
        }

        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());
        info.setLayoutCount = static_cast<uint32_t>(layouts.size());
        info.pPushConstantRanges = ranges.data();
        info.pSetLayouts = layouts.data();

        vkCreatePipelineLayout(pipelineLayout->device->device, &info, nullptr,
                               &pipelineLayout->layout);
        pipelineLayout->device->pipelineLayouts.push_back(pipelineLayout);

        return pipelineLayout;
    }

    GraphicsPipelineBuilder::GraphicsPipelineBuilder()
    {
        _impl = new GraphicsPipelineBuilderImpl;
    }

    GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
    {
        delete _impl;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::allowDerivatives()
    {
        _impl->allowDerivatives = true;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::deriveFrom(
        const IGraphicsPipeline* parent)
    {
        _impl->parent = const_cast<IGraphicsPipeline*>(parent);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::vertex(
        const IShaderModule* vertex)
    {
        _impl->vertex = const_cast<IShaderModule*>(vertex);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::fragment(
        const IShaderModule* fragment)
    {
        _impl->fragment = const_cast<IShaderModule*>(fragment);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::input(
        const VertexInputState& bindings)
    {
        _impl->bindings = bindings;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::assembly(
        const InputAssemblyState& assembly)
    {
        _impl->input = assembly;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::tessellation(
        const TessellationState& tessellation)
    {
        _impl->tessellation = tessellation;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::viewports(
        const ViewportState& viewport)
    {
        _impl->viewports = viewport;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::rasterization(
        const RasterizationState& raster)
    {
        _impl->rasterization = raster;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::multisample(
        const MultisampleState& multi)
    {
        _impl->multisample = multi;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::depthStencil(
        const DepthStencilState& depth)
    {
        _impl->depth = depth;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::colorBlend(
        const ColorBlendState& color)
    {
        _impl->colors = color;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::dynamic(
        const DynamicState& dynamic)
    {
        _impl->dynamic = dynamic;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::layout(
        const IPipelineLayout* layout)
    {
        _impl->layout = const_cast<IPipelineLayout*>(layout);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::subpass(
        const uint32_t subpass)
    {
        _impl->subpass = subpass;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::renderpass(
        const IRenderPass* renderpass)
    {
        _impl->renderpass = const_cast<IRenderPass*>(renderpass);
        return *this;
    }

    IGraphicsPipeline* GraphicsPipelineBuilder::build() const
    {
        VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline;
        pipeline->layout = cast<VulkanPipelineLayout*>(_impl->layout);
        pipeline->device = pipeline->layout->device;
        pipeline->renderpass = cast<VulkanRenderPass*>(_impl->renderpass);

        vector<VkPipelineShaderStageCreateInfo> stages;
        {
            if (_impl->vertex)
            {
                VkPipelineShaderStageCreateInfo stage = {};
                stage.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
                stage.module =
                    cast<VulkanShaderModule*>(_impl->vertex)->shaderModule;
                stage.pName = shader_entry.data();

                stages.push_back(stage);
            }

            if (_impl->fragment)
            {
                VkPipelineShaderStageCreateInfo stage = {};
                stage.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                stage.module =
                    cast<VulkanShaderModule*>(_impl->fragment)->shaderModule;
                stage.pName = shader_entry.data();

                stages.push_back(stage);
            }
        }

        vector<VkVertexInputAttributeDescription> vertexAttribs;
        vector<VkVertexInputBindingDescription> vertexDescs;

        for (const auto& attribBinding : _impl->bindings.bindings)
        {
            VkVertexInputAttributeDescription attrib;
            VkVertexInputBindingDescription desc;

            attrib.binding = attribBinding.binding;
            attrib.format = static_cast<VkFormat>(attribBinding.format);
            attrib.location = attribBinding.location;
            attrib.offset = attribBinding.offset;

            desc.binding = attribBinding.binding;
            desc.inputRate =
                static_cast<VkVertexInputRate>(attribBinding.inputRate);
            desc.stride = attribBinding.stride;

            bool attribFound = false;
            for (const auto a : vertexAttribs)
            {
                if (a.location == attrib.location)
                {
                    attribFound = true;
                }
            }

            bool descFound = false;
            for (const auto d : vertexDescs)
            {
                if (d.binding == attrib.binding)
                {
                    descFound = true;
                }
            }

            if (!attribFound)
            {
                vertexAttribs.push_back(attrib);
            }

            if (!descFound)
            {
                vertexDescs.push_back(desc);
            }
        }

        vector<VkViewport> viewports;
        vector<VkRect2D> scissors;

        for (const auto& viewport : _impl->viewports.viewports)
        {
            viewports.push_back({viewport.x, viewport.y, viewport.width,
                                 viewport.height, viewport.minDepth,
                                 viewport.maxDepth});
        }

        for (const auto& scissor : _impl->viewports.scissorRegions)
        {
            scissors.push_back(
                {{scissor.x, scissor.y}, {scissor.width, scissor.height}});
        }

        vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        for (const auto& attachment : _impl->colors.attachments)
        {
            colorBlendAttachmentStates.push_back(
                {static_cast<VkBool32>(attachment.blendEnable ? VK_TRUE
                                                              : VK_FALSE),
                 static_cast<VkBlendFactor>(attachment.srcColorBlendFactor),
                 static_cast<VkBlendFactor>(attachment.dstColorBlendFactor),
                 static_cast<VkBlendOp>(attachment.colorBlendOp),
                 static_cast<VkBlendFactor>(attachment.srcAlphaBlendFactor),
                 static_cast<VkBlendFactor>(attachment.dstAlphaBlendFactor),
                 static_cast<VkBlendOp>(attachment.alphaBlendOp),
                 attachment.colorMask});
        }

        vector<VkDynamicState> dynamicStates;
        for (const auto& state : _impl->dynamic.states)
        {
            dynamicStates.push_back(static_cast<VkDynamicState>(state));
        }

        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexAttribs.size());
        vertexInput.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexDescs.size());
        vertexInput.pVertexAttributeDescriptions = vertexAttribs.data();
        vertexInput.pVertexBindingDescriptions = vertexDescs.data();

        VkPipelineInputAssemblyStateCreateInfo assembly = {};
        assembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assembly.primitiveRestartEnable =
            _impl->input.primitiveRestartEnable ? VK_TRUE : VK_FALSE;
        assembly.topology =
            static_cast<VkPrimitiveTopology>(_impl->input.topology);

        VkPipelineTessellationStateCreateInfo tessellation = {};
        tessellation.sType =
            VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellation.patchControlPoints =
            _impl->tessellation.patchControlPoints;

        VkPipelineViewportStateCreateInfo viewport = {};
        viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.scissorCount = static_cast<uint32_t>(scissors.size());
        viewport.viewportCount = static_cast<uint32_t>(viewports.size());
        viewport.pScissors = scissors.data();
        viewport.pViewports = viewports.data();

        VkPipelineRasterizationStateCreateInfo rasterization = {};
        rasterization.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.cullMode = _impl->rasterization.cullMode;
        rasterization.depthBiasClamp = _impl->rasterization.depthBiasClamp;
        rasterization.depthBiasConstantFactor =
            _impl->rasterization.depthBiasConstantFactor;
        rasterization.depthBiasEnable =
            _impl->rasterization.depthBiasEnable ? VK_TRUE : VK_FALSE;
        rasterization.depthBiasSlopeFactor =
            _impl->rasterization.depthBiasSlopeFactor;
        rasterization.depthClampEnable =
            _impl->rasterization.depthClampEnable ? VK_TRUE : VK_FALSE;
        rasterization.frontFace =
            static_cast<VkFrontFace>(_impl->rasterization.frontFace);
        rasterization.lineWidth = _impl->rasterization.lineWidth;
        rasterization.polygonMode =
            static_cast<VkPolygonMode>(_impl->rasterization.mode);
        rasterization.rasterizerDiscardEnable =
            _impl->rasterization.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE;

        uint32_t sampleMask = _impl->multisample.sampleMask.has_value()
                                  ? _impl->multisample.sampleMask.value()
                                  : 0;
        VkPipelineMultisampleStateCreateInfo multisample = {};
        multisample.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.alphaToCoverageEnable =
            _impl->multisample.alphaToCoverage ? VK_TRUE : VK_FALSE;
        multisample.alphaToOneEnable =
            _impl->multisample.alphaToOne ? VK_TRUE : VK_FALSE;
        multisample.minSampleShading = _impl->multisample.minSampleShading;
        multisample.pSampleMask =
            _impl->multisample.sampleMask.has_value() ? &sampleMask : nullptr;
        multisample.rasterizationSamples =
            static_cast<VkSampleCountFlagBits>(_impl->multisample.samples);
        multisample.sampleShadingEnable =
            _impl->multisample.sampleShadingEnable ? VK_TRUE : VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depth = {};
        depth.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth.back = {static_cast<VkStencilOp>(_impl->depth.back.failOp),
                      static_cast<VkStencilOp>(_impl->depth.back.passOp),
                      static_cast<VkStencilOp>(_impl->depth.back.depthFailOp),
                      static_cast<VkCompareOp>(_impl->depth.back.compareOp),
                      _impl->depth.back.compareMask,
                      _impl->depth.back.writeMask,
                      _impl->depth.back.reference};
        depth.depthBoundsTestEnable =
            _impl->depth.depthBoundsTestEnable ? VK_TRUE : VK_FALSE;
        depth.depthCompareOp =
            static_cast<VkCompareOp>(_impl->depth.depthCompareOp);
        depth.depthTestEnable =
            _impl->depth.depthTestEnable ? VK_TRUE : VK_FALSE;
        depth.depthWriteEnable =
            _impl->depth.depthWriteEnable ? VK_TRUE : VK_FALSE;
        depth.front = {static_cast<VkStencilOp>(_impl->depth.front.failOp),
                       static_cast<VkStencilOp>(_impl->depth.front.passOp),
                       static_cast<VkStencilOp>(_impl->depth.front.depthFailOp),
                       static_cast<VkCompareOp>(_impl->depth.front.compareOp),
                       _impl->depth.front.compareMask,
                       _impl->depth.front.writeMask,
                       _impl->depth.front.reference};
        depth.maxDepthBounds = _impl->depth.maxDepthBounds;
        depth.minDepthBounds = _impl->depth.minDepthBounds;
        depth.stencilTestEnable = _impl->depth.stencilTestEnable;

        VkPipelineColorBlendStateCreateInfo colorBlend = {};
        colorBlend.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.attachmentCount =
            static_cast<uint32_t>(colorBlendAttachmentStates.size());
        memcpy(colorBlend.blendConstants, _impl->colors.blendConstants,
               4 * sizeof(float));
        colorBlend.logicOp = static_cast<VkLogicOp>(_impl->colors.logicOp);
        colorBlend.logicOpEnable =
            _impl->colors.logicEnable ? VK_TRUE : VK_FALSE;
        colorBlend.pAttachments = colorBlendAttachmentStates.data();

        VkPipelineDynamicStateCreateInfo dynamic = {};
        dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic.dynamicStateCount =
            static_cast<uint32_t>(_impl->dynamic.states.size());
        dynamic.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.flags |= _impl->allowDerivatives
                          ? VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT
                          : 0;
        info.flags |= _impl->parent ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0;
        info.stageCount = static_cast<uint32_t>(stages.size());
        info.pStages = stages.data();
        info.pVertexInputState = &vertexInput;
        info.pInputAssemblyState = &assembly;
        info.pTessellationState = &tessellation;
        info.pViewportState = &viewport;
        info.pRasterizationState = &rasterization;
        info.pMultisampleState = &multisample;
        info.pDepthStencilState = &depth;
        info.pColorBlendState = &colorBlend;
        info.pDynamicState = &dynamic;
        info.layout = pipeline->layout->layout;
        info.subpass = _impl->subpass;
        info.basePipelineHandle =
            _impl->parent
                ? cast<VulkanGraphicsPipeline*>(_impl->parent)->pipeline
                : nullptr;
        info.basePipelineIndex = 0;
        info.renderPass = pipeline->renderpass->renderpass;

        vkCreateGraphicsPipelines(pipeline->device->device, nullptr, 1, &info,
                                  nullptr, &pipeline->pipeline);
        pipeline->device->graphicsPipelines.push_back(pipeline);

        return pipeline;
    }

    RenderPassBuilder::RenderPassBuilder()
    {
        _impl = new RenderPassBuilderImpl;
    }

    RenderPassBuilder::~RenderPassBuilder()
    {
        delete _impl;
    }

    RenderPassBuilder& RenderPassBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::attachments(
        const vector<AttachmentDescription>& attachments)
    {
        _impl->attachments = attachments;
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::subpasses(
        const vector<SubpassDescription>& subpasses)
    {
        _impl->subpasses = subpasses;
        return *this;
    }

    RenderPassBuilder& RenderPassBuilder::dependencies(
        const vector<SubpassDependency>& dependencies)
    {
        _impl->dependencies = dependencies;
        return *this;
    }

    IRenderPass* RenderPassBuilder::build() const
    {
        VulkanRenderPass* pass = new VulkanRenderPass;
        pass->device = cast<VulkanDevice*>(_impl->device);

        vector<VkAttachmentDescription> attachments;
        vector<VkSubpassDescription> subpasses;
        vector<VkSubpassDependency> dependencies;

        vector<vector<VkAttachmentReference>> inputAttachments;
        vector<vector<VkAttachmentReference>> colorAttachments;
        vector<vector<VkAttachmentReference>> resolveAttachments;
        vector<VkAttachmentReference> depthAttachments;
        vector<vector<uint32_t>> preserveAttachments;

        attachments.reserve(_impl->attachments.size());
        subpasses.reserve(_impl->subpasses.size());
        dependencies.reserve(_impl->dependencies.size());
        inputAttachments.reserve(subpasses.size());
        colorAttachments.reserve(subpasses.size());
        resolveAttachments.reserve(subpasses.size());
        depthAttachments.reserve(subpasses.size());
        preserveAttachments.reserve(subpasses.size());

        for (const auto& sub : _impl->subpasses)
        {
            vector<VkAttachmentReference> inputs;
            vector<VkAttachmentReference> colors;
            vector<VkAttachmentReference> resolves;
            vector<uint32_t> preserve;
            VkAttachmentReference depth;

            for (const auto& ref : sub.inputAttachments)
            {
                inputs.push_back(
                    {ref.attachment, static_cast<VkImageLayout>(ref.layout)});
            }

            for (const auto& ref : sub.colorAttachments)
            {
                colors.push_back(
                    {ref.attachment, static_cast<VkImageLayout>(ref.layout)});
            }

            for (const auto& ref : sub.resolveAttachments)
            {
                resolves.push_back(
                    {ref.attachment, static_cast<VkImageLayout>(ref.layout)});
            }

            if (sub.depthAttachment.has_value())
            {
                depth = {sub.depthAttachment.value().attachment,
                         static_cast<VkImageLayout>(
                             sub.depthAttachment.value().layout)};
            }

            preserve = sub.preserveAttachments;

            inputAttachments.push_back(inputs);
            colorAttachments.push_back(colors);
            resolveAttachments.push_back(resolves);
            depthAttachments.push_back(depth);
            preserveAttachments.push_back(preserve);

            subpasses.push_back(
                {0, static_cast<VkPipelineBindPoint>(sub.bind),
                 static_cast<uint32_t>(inputs.size()),
                 inputAttachments[inputAttachments.size() - 1].data(),
                 static_cast<uint32_t>(colors.size()),
                 colorAttachments[colorAttachments.size() - 1].data(),
                 resolveAttachments[resolveAttachments.size() - 1].data(),
                 sub.depthAttachment.has_value()
                     ? &depthAttachments[depthAttachments.size() - 1]
                     : nullptr,
                 static_cast<uint32_t>(preserve.size()),
                 preserveAttachments[preserveAttachments.size() - 1].data()});
        }

        for (const auto& attachment : _impl->attachments)
        {
            attachments.push_back(
                {0, static_cast<VkFormat>(attachment.format),
                 static_cast<VkSampleCountFlagBits>(attachment.samples),
                 static_cast<VkAttachmentLoadOp>(attachment.loadOp),
                 static_cast<VkAttachmentStoreOp>(attachment.storeOp),
                 static_cast<VkAttachmentLoadOp>(attachment.stencilLoadOp),
                 static_cast<VkAttachmentStoreOp>(attachment.stencilStoreOp),
                 static_cast<VkImageLayout>(attachment.initialLayout),
                 static_cast<VkImageLayout>(attachment.finalLayout)});
        }

        for (const auto& dependency : _impl->dependencies)
        {
            dependencies.push_back(
                {dependency.srcSubpass, dependency.dstSubpass,
                 dependency.srcStageMask, dependency.dstStageMask,
                 dependency.srcAccessMask, dependency.dstAccessMask,
                 dependency.dependencyFlags});
        }

        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        createInfo.pDependencies = dependencies.data();
        createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        createInfo.pSubpasses = subpasses.data();

        vkCreateRenderPass(pass->device->device, &createInfo, nullptr,
                           &pass->renderpass);
        pass->device->renderPasses.push_back(pass);

        return pass;
    }

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

    CommandPoolBuilder::CommandPoolBuilder()
    {
        _impl = new CommandPoolBuilderImpl;
    }

    CommandPoolBuilder::~CommandPoolBuilder()
    {
        delete _impl;
    }

    CommandPoolBuilder& CommandPoolBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    CommandPoolBuilder& CommandPoolBuilder::queue(const IQueue* queue)
    {
        _impl->queue = const_cast<IQueue*>(queue);
        return *this;
    }

    CommandPoolBuilder& CommandPoolBuilder::transient()
    {
        _impl->transient = true;
        return *this;
    }

    CommandPoolBuilder& CommandPoolBuilder::reset()
    {
        _impl->reset = true;
        return *this;
    }

    ICommandPool* CommandPoolBuilder::build() const
    {
        VulkanCommandPool* pool = new VulkanCommandPool;
        pool->queue = cast<VulkanQueue*>(_impl->queue);
        pool->device = cast<VulkanDevice*>(_impl->device);

        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags |=
            _impl->transient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
        info.flags |=
            _impl->reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
        info.queueFamilyIndex = pool->queue->family.index;

        vkCreateCommandPool(pool->device->device, &info, nullptr, &pool->pool);
        pool->device->commandBufferPools.push_back(pool);

        return pool;
    }

    SemaphoreBuilder::SemaphoreBuilder()
    {
        _impl = new SemaphoreBuilderImpl;
    }

    SemaphoreBuilder::~SemaphoreBuilder()
    {
        delete _impl;
    }

    SemaphoreBuilder& SemaphoreBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    ISemaphore* SemaphoreBuilder::build() const
    {
        VulkanSemaphore* sem = new VulkanSemaphore;
        sem->device = cast<VulkanDevice*>(_impl->device);

        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(sem->device->device, &info, nullptr, &sem->semaphore);
        sem->device->sems.push_back(sem);

        return sem;
    }

    FenceBuilder::FenceBuilder()
    {
        _impl = new FenceBuilderImpl;
    }

    FenceBuilder::~FenceBuilder()
    {
        delete _impl;
    }

    FenceBuilder& FenceBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    FenceBuilder& FenceBuilder::signaled()
    {
        _impl->signalled = true;
        return *this;
    }

    IFence* FenceBuilder::build() const
    {
        VulkanFence* fence = new VulkanFence;

        VkFenceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.flags |= _impl->signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        fence->device = cast<VulkanDevice*>(_impl->device);
        vkCreateFence(fence->device->device, &info, nullptr, &fence->fence);
        fence->device->fences.push_back(fence);

        return fence;
    }

    VulkanContext::~VulkanContext()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (const auto& dev : devices)
            {
                delete dev;
            }

            if (debugMessenger)
            {
                const auto func =
                    reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                        vkGetInstanceProcAddr(
                            instance, "vkDestroyDebugUtilsMessengerEXT"));
                if (func != nullptr)
                {
                    func(instance, debugMessenger, nullptr);
                }
                else
                {
                    fprintf(stderr, "Failed to load function "
                                    "vkCreateDebugUtilsMessengerEXT.  "
                                    "Extension not present.");
                }
            }
            vkDestroyInstance(instance, nullptr);
        }
    }

    vector<IPhysicalDevice*> VulkanContext::physicalDevices() const
    {
        vector<IPhysicalDevice*> devs;

        for (const auto& dev : devices)
        {
            devs.push_back(dev);
        }

        return devs;
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!context->destroyed)
            {
                this->context->devices.erase(
                    std::find(this->context->devices.begin(),
                              this->context->devices.end(), this));
            }

            if (!logicalDevice->destroyed)
            {
                delete logicalDevice;
            }
        }
    }

    std::string VulkanPhysicalDevice::name() const
    {
        return deviceName;
    }

    EPhysicalDeviceType VulkanPhysicalDevice::type() const
    {
        return deviceType;
    }

    IPhysicalDevice::Features VulkanPhysicalDevice::features() const
    {
        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceFeatures(device, &feats);

        Features features = {
            feats.robustBufferAccess == VK_TRUE,
            feats.fullDrawIndexUint32 == VK_TRUE,
            feats.imageCubeArray == VK_TRUE,
            feats.independentBlend == VK_TRUE,
            feats.geometryShader == VK_TRUE,
            feats.tessellationShader == VK_TRUE,
            feats.sampleRateShading == VK_TRUE,
            feats.dualSrcBlend == VK_TRUE,
            feats.logicOp == VK_TRUE,
            feats.multiDrawIndirect == VK_TRUE,
            feats.drawIndirectFirstInstance == VK_TRUE,
            feats.depthClamp == VK_TRUE,
            feats.depthBiasClamp == VK_TRUE,
            feats.fillModeNonSolid == VK_TRUE,
            feats.depthBounds == VK_TRUE,
            feats.wideLines == VK_TRUE,
            feats.largePoints == VK_TRUE,
            feats.alphaToOne == VK_TRUE,
            feats.multiViewport == VK_TRUE,
            feats.samplerAnisotropy == VK_TRUE,
            feats.textureCompressionETC2 == VK_TRUE,
            feats.textureCompressionASTC_LDR == VK_TRUE,
            feats.textureCompressionBC == VK_TRUE,
            feats.occlusionQueryPrecise == VK_TRUE,
            feats.pipelineStatisticsQuery == VK_TRUE,
            feats.vertexPipelineStoresAndAtomics == VK_TRUE,
            feats.fragmentStoresAndAtomics == VK_TRUE,
            feats.shaderTessellationAndGeometryPointSize == VK_TRUE,
            feats.shaderImageGatherExtended == VK_TRUE,
            feats.shaderStorageImageExtendedFormats == VK_TRUE,
            feats.shaderStorageImageMultisample == VK_TRUE,
            feats.shaderStorageImageReadWithoutFormat == VK_TRUE,
            feats.shaderStorageImageWriteWithoutFormat == VK_TRUE,
            feats.shaderUniformBufferArrayDynamicIndexing == VK_TRUE,
            feats.shaderSampledImageArrayDynamicIndexing == VK_TRUE,
            feats.shaderStorageBufferArrayDynamicIndexing == VK_TRUE,
            feats.shaderStorageImageArrayDynamicIndexing == VK_TRUE,
            feats.shaderClipDistance == VK_TRUE,
            feats.shaderCullDistance == VK_TRUE,
            feats.shaderFloat64 == VK_TRUE,
            feats.shaderInt64 == VK_TRUE,
            feats.shaderInt16 == VK_TRUE,
            feats.shaderResourceResidency == VK_TRUE,
            feats.shaderResourceMinLod == VK_TRUE,
            feats.sparseBinding == VK_TRUE,
            feats.sparseResidencyBuffer == VK_TRUE,
            feats.sparseResidencyImage2D == VK_TRUE,
            feats.sparseResidencyImage2D == VK_TRUE,
            feats.sparseResidency2Samples == VK_TRUE,
            feats.sparseResidency4Samples == VK_TRUE,
            feats.sparseResidency8Samples == VK_TRUE,
            feats.sparseResidency16Samples == VK_TRUE,
            feats.sparseResidencyAliased == VK_TRUE,
            feats.variableMultisampleRate == VK_TRUE,
            feats.inheritedQueries == VK_TRUE};

        return features;
    }

    vector<IPhysicalDevice::QueueProperties> VulkanPhysicalDevice::
        queueProperties()
    {
        uint32_t propCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &propCount, nullptr);
        vector<VkQueueFamilyProperties> props(propCount);
        vector<QueueProperties> res;
        res.reserve(propCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &propCount,
                                                 props.data());

        uint32_t index = 0;
        for (const auto& prop : props)
        {
            QueueProperties p = {
                index++,
                (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_FALSE,
                (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) != VK_FALSE,
                (prop.queueFlags & VK_QUEUE_TRANSFER_BIT) != VK_FALSE,
                (prop.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != VK_FALSE,
                (prop.queueFlags & VK_QUEUE_PROTECTED_BIT) != VK_FALSE,
                prop.queueCount};
            res.push_back(p);
        }

        return res;
    }

    VulkanDevice::~VulkanDevice()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (const auto& fence : fences)
            {
                delete fence;
            }

            for (const auto& sem : sems)
            {
                delete sem;
            }

            for (const auto& pool : commandBufferPools)
            {
                delete pool;
            }

            for (const auto& gp : graphicsPipelines)
            {
                delete gp;
            }

            for (const auto& layout : pipelineLayouts)
            {
                delete layout;
            }

            for (const auto& pass : renderPasses)
            {
                delete pass;
            }

            for (const auto& layout : setLayouts)
            {
                delete layout;
            }

            for (const auto& mod : modules)
            {
                delete mod;
            }

            for (const auto& image : images)
            {
                delete image;
            }

            for (const auto& queue : deviceQueues)
            {
                delete queue;
            }

            for (const auto& surface : surfaces)
            {
                delete surface;
            }

            if (!parent->destroyed)
            {
                parent->device = nullptr;
                delete parent;
            }

            vkDestroyDevice(device, nullptr);
        }
    }

    vector<IQueue*> VulkanDevice::queues() const
    {
        return deviceQueues;
    }

    void VulkanDevice::idle() const
    {
        vkDeviceWaitIdle(device);
    }

    float VulkanQueue::priority() const
    {
        return queuePriority;
    }

    uint32_t VulkanQueue::index()
    {
        return queueIndex;
    }

    IPhysicalDevice::QueueProperties VulkanQueue::props() const
    {
        return family;
    }

    bool VulkanQueue::canPresent(const IPhysicalDevice* device,
                                 const ISurface* surface) const
    {
        VkBool32 present;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            cast<const VulkanPhysicalDevice*>(device)->device, family.index,
            cast<const VulkanSurface*>(surface)->surface, &present);
        return present == VK_TRUE;
    }

    void VulkanQueue::submit(const vector<SubmitInfo>& submitInfo,
                             const IFence* fence) const
    {
        vector<VkSubmitInfo> infos;
        infos.reserve(submitInfo.size());

        vector<vector<VkCommandBuffer>> buffers;
        vector<vector<VkSemaphore>> wait;
        vector<vector<VkSemaphore>> signal;
        vector<vector<VkPipelineStageFlags>> stages;

        buffers.reserve(submitInfo.size());
        wait.reserve(submitInfo.size());
        signal.reserve(submitInfo.size());
        stages.reserve(submitInfo.size());

        for (const auto& si : submitInfo)
        {
            vector<VkCommandBuffer> bufs;
            bufs.reserve(si.buffers.size());

            vector<VkSemaphore> waits;
            waits.reserve(si.wait.size());

            vector<VkSemaphore> signals;
            signals.reserve(si.signal.size());

            vector<VkPipelineStageFlags> flags;
            flags.reserve(si.waitMask.size());

            for (const auto& buf : si.buffers)
            {
                bufs.push_back(cast<VulkanCommandBuffer*>(buf)->buffer);
            }

            for (const auto& sem : si.wait)
            {
                waits.push_back(cast<VulkanSemaphore*>(sem)->semaphore);
            }

            for (const auto& sem : si.signal)
            {
                signals.push_back(cast<VulkanSemaphore*>(sem)->semaphore);
            }

            for (const EPipelineStageFlags flag : si.waitMask)
            {
                flags.push_back(static_cast<VkPipelineStageFlags>(flag));
            }

            buffers.push_back(bufs);
            wait.push_back(waits);
            signal.push_back(signals);
            stages.push_back(flags);

            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = static_cast<uint32_t>(waits.size());
            info.pWaitSemaphores = wait[wait.size() - 1].data();
            info.pWaitDstStageMask = stages[stages.size() - 1].data();
            info.commandBufferCount = static_cast<uint32_t>(buffers.size());
            info.pCommandBuffers = buffers[buffers.size() - 1].data();
            info.signalSemaphoreCount = static_cast<uint32_t>(signal.size());
            info.pSignalSemaphores = signal[signal.size() - 1].data();

            infos.push_back(info);
        }

        vkQueueSubmit(queue, static_cast<uint32_t>(infos.size()), infos.data(),
                      cast<const VulkanFence*>(fence)->fence);
    }

    void VulkanQueue::present(const PresentInfo& presentInfo) const
    {
        vector<VkSemaphore> waits;
        VkSwapchainKHR swapchain =
            cast<VulkanSwapchain*>(presentInfo.swapchain)->swapchain;
        uint32_t index = presentInfo.image;

        for (const auto& sem : presentInfo.waits)
        {
            waits.push_back(cast<VulkanSemaphore*>(sem)->semaphore);
        }

        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = static_cast<uint32_t>(waits.size());
        info.pWaitSemaphores = waits.data();
        info.swapchainCount = 1;
        info.pSwapchains = &swapchain;
        info.pImageIndices = &index;

        vkQueuePresentKHR(queue, &info);
    }

    VulkanSurface::~VulkanSurface()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->surfaces.erase(std::find(device->surfaces.begin(),
                                                 device->surfaces.end(), this));
            }

            if (!swapchain->destroyed)
            {
                delete swapchain;
            }
            vkDestroySurfaceKHR(context->instance, surface, nullptr);
        }
    }

    IWindow* VulkanSurface::window() const
    {
        return windowPtr;
    }

    ISurface::SwapchainSupport VulkanSurface::swapchainSupport(
        const IPhysicalDevice* device) const
    {
        const auto d = cast<const VulkanPhysicalDevice*>(device);

        VkSurfaceCapabilitiesKHR surfaceCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(d->device, surface,
                                                  &surfaceCaps);

        vector<SurfaceFormat> formats;
        vector<EPresentMode> presentModes;

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(d->device, surface,
                                                  &presentModeCount, nullptr);
        vector<VkPresentModeKHR> vkPresentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            d->device, surface, &presentModeCount, vkPresentModes.data());

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(d->device, surface, &formatCount,
                                             nullptr);
        vector<VkSurfaceFormatKHR> surfFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(d->device, surface, &formatCount,
                                             surfFormats.data());
        for (const auto format : surfFormats)
        {
            formats.push_back({static_cast<EFormat>(format.format),
                               static_cast<EColorSpace>(format.colorSpace)});
        }

        for (const auto& mode : vkPresentModes)
        {
            presentModes.push_back(static_cast<EPresentMode>(mode));
        }

        return {surfaceCaps.minImageCount,
                surfaceCaps.maxImageCount,
                surfaceCaps.currentExtent.width,
                surfaceCaps.currentExtent.height,
                surfaceCaps.minImageExtent.width,
                surfaceCaps.minImageExtent.height,
                surfaceCaps.maxImageExtent.width,
                surfaceCaps.maxImageExtent.height,
                surfaceCaps.maxImageArrayLayers,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != VK_FALSE,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) != VK_FALSE,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) != VK_FALSE,
                (surfaceCaps.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != VK_FALSE,
                surfaceCaps.supportedUsageFlags,
                static_cast<ESurfaceTransformFlagBits>(
                    surfaceCaps.currentTransform),
                surfaceCaps.supportedTransforms,
                formats,
                presentModes};
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        if (!destroyed)
        {
            destroyed = true;

            imgViews.clear();
            if (!surface->destroyed)
            {
                delete surface;
            }
            vkDestroySwapchainKHR(device->device, swapchain, nullptr);
            surface->swapchain = nullptr;
        }
    }

    uint32_t VulkanSwapchain::imagesCount() const
    {
        return static_cast<uint32_t>(imgViews.size());
    }

    vector<IImageView*> VulkanSwapchain::views() const
    {
        return imgViews;
    }

    EFormat VulkanSwapchain::format() const
    {
        return fmt;
    }

    uint32_t VulkanSwapchain::acquireNextImage(const uint64_t wait,
                                               const ISemaphore* signal,
                                               const IFence* fence)
    {
        const VulkanSemaphore* vkSem = cast<const VulkanSemaphore*>(signal);
        const VkSemaphore sem = vkSem ? vkSem->semaphore : VK_NULL_HANDLE;

        const VulkanFence* vkFence = cast<const VulkanFence*>(fence);
        const VkFence fen = vkFence ? vkFence->fence : VK_NULL_HANDLE;

        uint32_t index = UINT32_MAX;
        vkAcquireNextImageKHR(device->device, swapchain, wait, sem, fen,
                              &index);
        return index;
    }

    VulkanImage::~VulkanImage()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (auto view : views)
            {
                delete view;
            }

            if (owned)
            {
                vkDestroyImage(device->device, image, nullptr);
            }

            if (!device->destroyed)
            {
                device->images.erase(std::find(device->images.begin(),
                                               device->images.end(), this));
            }
        }
    }

    VulkanImageView::~VulkanImageView()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!image->destroyed)
            {
                image->views.erase(
                    std::find(image->views.begin(), image->views.end(), this));
            }
            vkDestroyImageView(image->device->device, view, nullptr);
        }
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->modules.erase(std::find(device->modules.begin(),
                                                device->modules.end(), this));
            }

            vkDestroyShaderModule(device->device, shaderModule, nullptr);
        }
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->setLayouts.erase(std::find(device->setLayouts.begin(),
                                                   device->setLayouts.end(),
                                                   this));
            }

            vkDestroyDescriptorSetLayout(device->device, layout, nullptr);
        }
    }

    vector<DescriptorSetLayoutBinding> VulkanDescriptorSetLayout::bindings()
        const
    {
        return layoutBindings;
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->pipelineLayouts.erase(
                    std::find(device->pipelineLayouts.begin(),
                              device->pipelineLayouts.end(), this));
            }

            vkDestroyPipelineLayout(device->device, layout, nullptr);
        }
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        if (!destroyed)
        {
            if (!device->destroyed)
            {
                device->graphicsPipelines.erase(
                    std::find(device->graphicsPipelines.begin(),
                              device->graphicsPipelines.end(), this));
            }

            vkDestroyPipeline(device->device, pipeline, nullptr);
        }
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

    VulkanRenderPass::~VulkanRenderPass()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (const auto& fb : framebuffers)
            {
                delete fb;
            }

            if (!device->destroyed)
            {
                device->renderPasses.erase(
                    std::find(device->renderPasses.begin(),
                              device->renderPasses.end(), this));
            }

            vkDestroyRenderPass(device->device, renderpass, nullptr);
        }
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->commandBufferPools.erase(
                    std::find(device->commandBufferPools.begin(),
                              device->commandBufferPools.end(), this));
            }

            for (const auto& buf : buffers)
            {
                delete buf;
            }

            vkDestroyCommandPool(device->device, pool, nullptr);
        }
    }

    ICommandBuffer* VulkanCommandPool::allocate(const ECommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandBufferCount = 1;
        info.commandPool = pool;
        info.level = level == ECommandBufferLevel::PRIMARY
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        VkCommandBuffer buf;
        vkAllocateCommandBuffers(device->device, &info, &buf);

        VulkanCommandBuffer* buffer = new VulkanCommandBuffer;
        buffer->buffer = buf;
        buffer->pool = this;

        return buffer;
    }

    vector<ICommandBuffer*> VulkanCommandPool::allocate(
        const uint32_t count, const ECommandBufferLevel level)
    {
        vector<VkCommandBuffer> bufs(count);

        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandBufferCount = count;
        info.commandPool = pool;
        info.level = level == ECommandBufferLevel::PRIMARY
                         ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                         : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        vkAllocateCommandBuffers(device->device, &info, bufs.data());

        vector<ICommandBuffer*> buffers;
        buffers.reserve(count);

        for (const auto& buf : bufs)
        {
            const auto buffer = new VulkanCommandBuffer;
            buffer->buffer = buf;
            buffer->pool = this;
            buffers.push_back(buffer);
        }

        return buffers;
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!pool->destroyed)
            {
                pool->buffers.erase(std::find(pool->buffers.begin(),
                                              pool->buffers.end(), this));
            }

            vkFreeCommandBuffers(pool->device->device, pool->pool, 1, &buffer);
        }
    }

    void VulkanCommandBuffer::record() const
    {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        vkBeginCommandBuffer(buffer, &info);
    }

    void VulkanCommandBuffer::end() const
    {
        vkEndCommandBuffer(buffer);
    }

    void VulkanCommandBuffer::beginRenderPass(const RenderPassRecordInfo& info,
                                              const bool isInline)
    {
        vector<VkClearValue> values;
        for (const auto& value : info.clearValues)
        {
            VkClearValue v;
            memcpy(&v, &value, sizeof(VkClearValue));
            values.push_back(v);
        }

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.framebuffer = cast<VulkanFramebuffer*>(info.renderTarget)->fb;
        beginInfo.renderArea = {{info.x, info.y}, {info.width, info.height}};
        beginInfo.renderPass =
            cast<VulkanRenderPass*>(info.renderpass)->renderpass;
        beginInfo.clearValueCount = static_cast<uint32_t>(values.size());
        beginInfo.pClearValues = values.data();

        vkCmdBeginRenderPass(
            buffer, &beginInfo,
            isInline ? VK_SUBPASS_CONTENTS_INLINE
                     : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    }

    void VulkanCommandBuffer::endRenderPass()
    {
        vkCmdEndRenderPass(buffer);
    }

    void VulkanCommandBuffer::draw(const uint32_t vertices,
                                   const uint32_t instances,
                                   const uint32_t baseVertex,
                                   const uint32_t baseInstance)
    {
        vkCmdDraw(buffer, vertices, instances, baseVertex, baseInstance);
    }

    void VulkanCommandBuffer::bind(const IGraphicsPipeline* pipeline)
    {
        vkCmdBindPipeline(
            buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            cast<const VulkanGraphicsPipeline*>(pipeline)->pipeline);
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->sems.erase(
                    std::find(device->sems.begin(), device->sems.end(), this));
            }

            vkDestroySemaphore(device->device, semaphore, nullptr);
        }
    }

    VulkanFence::~VulkanFence()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->fences.erase(std::find(device->fences.begin(),
                                               device->fences.end(), this));
            }

            vkDestroyFence(device->device, fence, nullptr);
        }
    }

    void VulkanFence::reset()
    {
        vkResetFences(device->device, 1, &fence);
    }

    void VulkanFence::wait()
    {
        vkWaitForFences(device->device, 1, &fence, VK_TRUE, UINT64_MAX);
    }

    bool VulkanFence::status()
    {
        return vkGetFenceStatus(device->device, fence) == VK_SUCCESS;
    }
} // namespace helios
