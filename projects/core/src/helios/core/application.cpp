#include <helios/core/application.hpp>

#include <helios/core/engine_context.hpp>

namespace helios
{
    Application::Application()
    {
        EngineContext::_initialize();
    }

    Application::~Application()
    {
        EngineContext::_close();
    }
}