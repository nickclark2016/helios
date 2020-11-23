#include <helios/core/application.hpp>

#include <helios/core/engine_context.hpp>

namespace helios
{
    Application::Application()
    {
        EngineContextFactory().create();
    }

    Application::~Application()
    {
        EngineContextFactory().release();
    }
}