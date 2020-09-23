#pragma once

#include <string>

namespace helios
{
    class Application
    {
    public:
        Application();
        virtual ~Application();
        virtual void initialize() = 0;
    };

    Application* CreateApplication();
} // namespace helios