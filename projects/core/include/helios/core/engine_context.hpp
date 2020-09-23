#pragma once

#include <helios/core/window.hpp>

namespace helios
{
	class EngineContext
    {
    public:
        static EngineContext& instance();

        IWindow& window();

    private:
        IWindow* _win;

        friend class Application;
        static void _initialize();
        static void _close();
        static EngineContext* _ctx;
    };
}