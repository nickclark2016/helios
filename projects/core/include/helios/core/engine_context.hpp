#pragma once

#include <helios/containers/vector.hpp>
#include <helios/core/window.hpp>
#include <helios/ecs/entity.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/render/resource_manager.hpp>

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

        struct Viewport
        {
            f32 x;
            f32 y;
            f32 width;
            f32 height;
            f32 minDepth;
            f32 maxDepth;
        };

        class RenderContext
        {
            friend class EngineContext;
        public:
            RenderContext() = default;
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
            ResourceManager& resources();
            void reset();

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
            ResourceManager* _resourceManager;
            FrameInfo _frameInfo;
            u32 _framesInFlight;
            EngineContext* _engineCtx;
        };

        EngineContext();
        ~EngineContext() = default;
        HELIOS_NO_COPY_MOVE(EngineContext)

        virtual IWindow& window();
        virtual RenderContext& render();
        virtual EntityManager& entities();

    private:
        IWindow* _win;
        RenderContext* _render;
        EntityManager* _entities;

        void _initialize();
        void _close();
    };

    class EngineContextFactory
    {
    public:
        EngineContext& create();
    private:

        friend class Application;
        bool release();
    };
}