#include <helios/core/engine_context.hpp>

#include <helios/io/file.hpp>

#include <nlohmann/json.hpp>

#include <mutex>
#include <thread>

namespace helios
{
    static u32 getThreadId()
    {
        static thread_local u32 id = 0;
        static thread_local bool hasId = false;
        if (hasId == false)
        {
            static std::mutex mx;
            static u32 counter = 0;

            std::lock_guard<std::mutex> lock(mx);
            id = counter++;
        }
        return id;
    }

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
        delete _resourceManager;
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

    ISemaphore& EngineContext::RenderContext::imageAvailableSync() const noexcept
    {
        return *(_imagesReady[_frameInfo.resourceIndex]);
    }

    const EngineContext::FrameInfo& EngineContext::RenderContext::currentFrame() const noexcept
    {
        return _frameInfo;
    }

    void EngineContext::RenderContext::nextFrame() noexcept
    {
        _frameInfo.resourceIndex = (_frameInfo.resourceIndex + 1) % _framesInFlight;
    }

    void EngineContext::RenderContext::startFrame()
    {
        const u32 swapchainIndex = _swapchain->acquireNextImage(UINT64_MAX, &imageAvailableSync(), nullptr);
        _frameInfo.swapchainIndex = swapchainIndex;

        for (auto& bufferedPool : _bufferedCommandPool)
        {
            bufferedPool.bufferIndex = 0;
        }
    }

    ICommandBuffer& EngineContext::RenderContext::getCommandBuffer()
    {
        // Main thread gets index 0, worker 1 gets index 1, etc.
        const i32 id = getThreadId();
        BufferedCommandPool& pool = _bufferedCommandPool[id];
        vector<ICommandBuffer*>& buffers = pool.buffers[_frameInfo.resourceIndex];
        if (buffers.size() <= pool.bufferIndex)
        {
            buffers.push_back(pool.pool->allocate());
        }
        return *buffers[pool.bufferIndex++];
    }

    ResourceManager& EngineContext::RenderContext::resources()
    {
        return *_resourceManager;
    }

    void EngineContext::RenderContext::reset()
    {
        _imagesReady.clear();

        for (u32 i = 0; i < _framesInFlight; ++i)
        {
            ISemaphore* sem = SemaphoreBuilder().device(_device).build();
            _imagesReady.push_back(sem);
        }

        u32 poolCount = _bufferedCommandPool.size();
        _bufferedCommandPool.clear();

        for (u32 i = 0; i < poolCount + 1; ++i)
        {
            RenderContext::BufferedCommandPool pool;
            pool.pool = CommandPoolBuilder()
                            .device(_device)
                            .reset()
                            .queue(&graphicsQueue())
                            .build();
            pool.bufferIndex = 0;
            pool.buffers.resize(_framesInFlight);
            _bufferedCommandPool.push_back(pool);
        }
    }

    EngineContext::EngineContext()
    {
        _initialize();
    }

    IWindow& EngineContext::window()
    {
        return *_win;
    }

    EngineContext::RenderContext& EngineContext::render()
    {
        return *_render;
    }

    EntityManager& EngineContext::entities()
    {
        return *_entities;
    }

    void EngineContext::_initialize()
    {
        using nlohmann::json;

        json configuration = json::parse(File::read_text("assets/config.json"));
        auto& appConfiguration = configuration["application"];
        auto& windowConfiguration = appConfiguration["window"];

        _win = WindowBuilder().title(windowConfiguration["title"])
            .width(windowConfiguration["width"])
            .height(windowConfiguration["height"])
            .resizable(windowConfiguration["resize"])
            .build();

        auto& engineConfiguration = configuration["engine"];

        _render = new EngineContext::RenderContext;
        _render->_engineCtx = this;
        ContextBuilder renderCtxBuilder;
        renderCtxBuilder.applicationName(appConfiguration["name"])
            .applicationVersion(appConfiguration["version"]["major"], appConfiguration["version"]["minor"], appConfiguration["version"]["patch"])
            .engineName(engineConfiguration["name"])
            .engineVersion(engineConfiguration["version"]["major"], engineConfiguration["version"]["minor"], engineConfiguration["version"]["patch"]);
#if defined(_DEBUG)
        renderCtxBuilder.validation();
#endif
        
        _render->_ctx = renderCtxBuilder.build();
        _render->_physicalDevice = _render->_ctx->physicalDevices()[0];
        
        DeviceBuilder deviceBuilder;
        deviceBuilder.physical(_render->_physicalDevice)
            .compute(engineConfiguration["graphics"]["computeQueueCount"])
            .graphics(engineConfiguration["graphics"]["graphicsQueueCount"])
            .transfer(engineConfiguration["graphics"]["transferQueueCount"])
            .swapchain();
#if defined(_DEBUG)
        deviceBuilder.validation();
#endif
        _render->_device = deviceBuilder.build();
        _render->_surface = SurfaceBuilder().device(_render->_device)
            .window(_win)
            .build();

        for (const auto& queue : _render->_device->queues())
        {
            if (queue->canPresent(_render->_physicalDevice, _render->_surface))
            {
                _render->_presentQueue = queue;
                break;
            }
        }

        for (const auto& queue : _render->_device->queues())
        {
            if (queue->props().graphics)
            {
                _render->_graphicsQueues.push_back(queue);
            }

            if (queue->props().transfer)
            {
                _render->_transferQueues.push_back(queue);
            }
        }

        _render->_frameInfo.resourceIndex = 0;
        _render->_framesInFlight = engineConfiguration["graphics"]["swapchainImageCount"];
        const auto swapchainSupport = _render->_surface->swapchainSupport(_render->_physicalDevice);
        _render->_swapchain = SwapchainBuilder()
            .surface(_render->_surface)
            .images(_render->_framesInFlight)
            .width(_win->width())
            .height(_win->height())
            .layers(1)
            .present(get_best_present_mode(swapchainSupport.presentModes))
            .format(get_best_surface_format(swapchainSupport.surfaceFormats).format)
            .colorSpace(get_best_surface_format(swapchainSupport.surfaceFormats).colorSpace)
            .queues({ _render->_presentQueue })
            .usage(IMAGE_COLOR_ATTACHMENT)
            .transform(swapchainSupport.currentTransform)
            .alphaOpaque()
            .clipped()
            .build();

        for (u32 i = 0; i < _render->_framesInFlight; ++i)
        {
            ISemaphore* sem = SemaphoreBuilder().device(_render->_device).build();
            _render->_imagesReady.push_back(sem);
        }

        const auto hasMinThreads = engineConfiguration["tasking"].contains("min");
        const auto hasMaxThreads = engineConfiguration["tasking"].contains("max");
        const u32 hardwareThreads = std::thread::hardware_concurrency() - 1; // Subtract 1 for main thread, which isn't part of the pool

        u32 requestedThreadCount = hardwareThreads;
        if (hasMinThreads)
        {
            requestedThreadCount = max(requestedThreadCount, (u32)engineConfiguration["tasking"]["min"]);
        }
        if (hasMaxThreads)
        {
            requestedThreadCount = min(requestedThreadCount, (u32)engineConfiguration["tasking"]["max"]);
        }

        for (u32 i = 0; i < hardwareThreads + 1; ++i)
        {
            RenderContext::BufferedCommandPool pool;
            pool.pool = CommandPoolBuilder()
                            .device(_render->_device)
                            .reset()
                            .queue(&_render->graphicsQueue())
                            .build();
            pool.bufferIndex = 0;
            pool.buffers.resize(_render->_framesInFlight);
            _render->_bufferedCommandPool.push_back(pool);
        }

        _render->_resourceManager = new ResourceManager();

        _entities = new EntityManager();
    }

    void EngineContext::_close()
    {
        delete _render;
        delete _win;
    }

    EngineContext& EngineContextFactory::create()
    {
        static EngineContext* context = new EngineContext();
        return *context;
    }

    bool EngineContextFactory::release()
    {
        static bool released = false;
        if (released == false)
        {
            delete &create();
            released = true;
        }

        return true;
    }
} // namespace helios