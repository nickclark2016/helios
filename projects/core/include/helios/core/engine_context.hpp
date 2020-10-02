#pragma once

#include <helios/containers/vector.hpp>
#include <helios/core/window.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

#include <taskflow/taskflow.hpp>

namespace helios
{
	class EngineContext
    {
    public:
        struct FrameInfo
        {
            u32 resourceIndex;
            u32 swapchainIndex;
        };

        class RenderContext
        {
            friend class EngineContext;
            RenderContext() = default;
        public:
            ~RenderContext();
            HELIOS_NO_COPY_MOVE(RenderContext);

            IContext& context();
            IDevice& device();
            ISwapchain& swapchain();
            IQueue& presentQueue();
            IQueue& transferQueue(const u32 idx = 0);
            IQueue& graphicsQueue(const u32 idx = 0);
            u32 transferQueueCount() const noexcept;
            u32 graphicsQueueCount() const noexcept;
            ISemaphore& imageAvailableSync() const noexcept;
            const FrameInfo& currentFrame() const noexcept;
            void nextFrame() noexcept;
            void startFrame();
            ICommandBuffer& getCommandBuffer();

        private:
            struct BufferedCommandPool
            {
                ICommandPool* pool;
                // outer vector - per frame
                // inner vector - command buffers allocated to the frame
                vector<vector<ICommandBuffer*>> buffers;
                // Index being currently accessed in the buffers
                u32 bufferIndex;
            };

            IContext* _ctx;
            IPhysicalDevice* _physicalDevice;
            IDevice* _device;
            ISurface* _surface;
            ISwapchain* _swapchain;
            IQueue* _presentQueue;
            vector<IQueue*> _transferQueues;
            vector<IQueue*> _graphicsQueues;
            vector<ISemaphore*> _imagesReady;
            vector<BufferedCommandPool> _bufferedCommandPool;
            FrameInfo _frameInfo;
            u32 _framesInFlight;
        };

        static EngineContext& instance();

        IWindow& window();
        RenderContext& render();
        Executor& tasks();

    private:
        IWindow* _win;
        RenderContext* _render;
        Executor* _taskExecutor;

        friend class Application;
        static void _initialize();
        static void _close();
        static EngineContext* _ctx;
    };
}