#include <helios/core/engine_context.hpp>

#include <helios/io/file.hpp>

#include <nlohmann/json.hpp>

namespace helios
{
    static EPresentMode get_best_present_mode(const vector<EPresentMode>& supported)
    {
        for (const auto mode : supported)
        {
            if (mode == EPresentMode::MAILBOX)
            {
                return mode;
            }
        }
        return EPresentMode::FIFO;
    }

    static ISurface::SurfaceFormat get_best_surface_format(const vector<ISurface::SurfaceFormat>& formats)
    {
        for (const auto& format : formats)
        {
            if (format.format == EFormat::B8G8R8A8_SRGB &&
                format.colorSpace == EColorSpace::SRGB_NONLINEAR)
            {
                return format;
            }
        }
        return formats[0];
    }

    EngineContext::RenderContext::~RenderContext()
    {
        delete _ctx;
    }

    IContext& EngineContext::RenderContext::context()
    {
        return *_ctx;
    }

    IDevice& EngineContext::RenderContext::device()
    {
        return *_device;
    }

    ISwapchain& EngineContext::RenderContext::swapchain()
    {
        return *_swapchain;
    }

    IQueue& EngineContext::RenderContext::presentQueue()
    {
        return *_presentQueue;
    }

    IQueue& EngineContext::RenderContext::transferQueue(const u32 idx)
    {
        if (idx < _transferQueues.size())
        {
            return *(_transferQueues[idx]);
        }
        return *(_transferQueues[0]);
    }

    IQueue& EngineContext::RenderContext::graphicsQueue(const u32 idx)
    {
        if (idx < _graphicsQueues.size())
        {
            return *(_graphicsQueues[idx]);
        }
        return *(_graphicsQueues[0]);
    }

    u32 EngineContext::RenderContext::transferQueueCount() const noexcept {
        return static_cast<u32>(_transferQueues.size());
    }

    u32 EngineContext::RenderContext::graphicsQueueCount() const noexcept
    {
        return static_cast<u32>(_graphicsQueues.size());
    }

    const EngineContext::FrameInfo& EngineContext::RenderContext::currentFrame() const noexcept
    {
        return _frameInfo;
    }

    void EngineContext::RenderContext::nextFrame() noexcept
    {
        _frameInfo.resourceIndex = (_frameInfo.resourceIndex + 1) % _framesInFlight;
    }

    void EngineContext::RenderContext::startFrame(ISemaphore& signal)
    {
        const u32 swapchainIndex = _swapchain->acquireNextImage(UINT64_MAX, &signal, nullptr);
        _frameInfo.swapchainIndex = swapchainIndex;
    }

    EngineContext* EngineContext::_ctx = nullptr;

    EngineContext& EngineContext::instance()
    {
        return *_ctx;
    }

    IWindow& EngineContext::window()
    {
        return *_win;
    }

    EngineContext::RenderContext& EngineContext::render()
    {
        return *(_ctx->_render);
    }

    void EngineContext::_initialize()
    {
        using nlohmann::json;

        json configuration = json::parse(File::read_text("assets/config.json"));
        auto& appConfiguration = configuration["application"];
        auto& windowConfiguration = appConfiguration["window"];

        _ctx = new EngineContext;
        _ctx->_win = WindowBuilder().title(windowConfiguration["title"])
            .width(windowConfiguration["width"])
            .height(windowConfiguration["height"])
            .resizable(windowConfiguration["resize"])
            .build();

        auto& engineConfiguration = configuration["engine"];

        _ctx->_render = new EngineContext::RenderContext;
        ContextBuilder renderCtxBuilder;
        renderCtxBuilder.applicationName(appConfiguration["name"])
            .applicationVersion(appConfiguration["version"]["major"], appConfiguration["version"]["minor"], appConfiguration["version"]["patch"])
            .engineName(engineConfiguration["name"])
            .engineVersion(engineConfiguration["version"]["major"], engineConfiguration["version"]["minor"], engineConfiguration["version"]["patch"]);
#if defined(_DEBUG)
        renderCtxBuilder.validation();
#endif
        
        _ctx->_render->_ctx = renderCtxBuilder.build();
        _ctx->_render->_physicalDevice = _ctx->_render->_ctx->physicalDevices()[0];
        
        DeviceBuilder deviceBuilder;
        deviceBuilder.physical(_ctx->_render->_physicalDevice)
            .compute(engineConfiguration["graphics"]["computeQueueCount"])
            .graphics(engineConfiguration["graphics"]["graphicsQueueCount"])
            .transfer(engineConfiguration["graphics"]["transferQueueCount"])
            .swapchain();
#if defined(_DEBUG)
        deviceBuilder.validation();
#endif
        _ctx->_render->_device = deviceBuilder.build();
        _ctx->_render->_surface = SurfaceBuilder().device(_ctx->_render->_device)
            .window(_ctx->_win)
            .build();

        for (const auto& queue : _ctx->_render->_device->queues())
        {
            if (queue->canPresent(_ctx->_render->_physicalDevice, _ctx->_render->_surface))
            {
                _ctx->_render->_presentQueue = queue;
                break;
            }
        }

        for (const auto& queue : _ctx->_render->_device->queues())
        {
            if (queue->props().graphics)
            {
                _ctx->_render->_graphicsQueues.push_back(queue);
            }

            if (queue->props().transfer)
            {
                _ctx->_render->_transferQueues.push_back(queue);
            }
        }

        _ctx->_render->_frameInfo.resourceIndex = 0;
        _ctx->_render->_framesInFlight = engineConfiguration["graphics"]["swapchainImageCount"];
        const auto swapchainSupport = _ctx->_render->_surface->swapchainSupport(_ctx->_render->_physicalDevice);
        _ctx->_render->_swapchain = SwapchainBuilder()
            .surface(_ctx->_render->_surface)
            .images(_ctx->_render->_framesInFlight)
            .width(_ctx->_win->width())
            .height(_ctx->_win->height())
            .layers(1)
            .present(get_best_present_mode(swapchainSupport.presentModes))
            .format(get_best_surface_format(swapchainSupport.surfaceFormats).format)
            .colorSpace(get_best_surface_format(swapchainSupport.surfaceFormats).colorSpace)
            .queues({ _ctx->_render->_presentQueue })
            .usage(IMAGE_COLOR_ATTACHMENT)
            .transform(swapchainSupport.currentTransform)
            .alphaOpaque()
            .clipped()
            .build();
    }

    void EngineContext::_close()
    {
        delete _ctx->_render;
        delete _ctx->_win;
        delete _ctx;
    }
} // namespace helios