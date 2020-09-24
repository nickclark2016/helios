#pragma once

#include <helios/core/window.hpp>
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

        private:
            EngineContext* _engine;
            IContext* _ctx;
            IPhysicalDevice* _physicalDevice;
            IDevice* _device;
            ISurface* _surface;
            ISwapchain* _swapchain;
            IQueue* _presentQueue;

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