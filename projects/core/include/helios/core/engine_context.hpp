#pragma once

#include <helios/containers/vector.hpp>
#include <helios/core/window.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
	class EngineContext
    {
    public:
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
            u32 currentFrame() const noexcept;
            void nextFrame() noexcept;

        private:
            IContext* _ctx;
            IPhysicalDevice* _physicalDevice;
            IDevice* _device;
            ISurface* _surface;
            ISwapchain* _swapchain;
            IQueue* _presentQueue;
            vector<IQueue*> _transferQueues;
            vector<IQueue*> _graphicsQueues;
            u32 _currentFrame;
            u32 _framesInFlight;
        };

        static EngineContext& instance();

        IWindow& window();
        RenderContext& render();

    private:
        IWindow* _win;
        RenderContext* _render;

        friend class Application;
        static void _initialize();
        static void _close();
        static EngineContext* _ctx;
    };
}