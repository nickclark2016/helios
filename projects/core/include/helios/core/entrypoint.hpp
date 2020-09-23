#pragma once

#include <helios/core/application.hpp>

#define DEFAULT_ENTRYPOINT(Type)                                                                                      \
    helios::Application* helios::CreateApplication()                                                                   \
    {                                                                                                                  \
        return new Type();                                                                                             \
    }

extern helios::Application* helios::CreateApplication();

int main(int argc, char** argv)
{
    using helios::Application;
    using helios::CreateApplication;

    Application* app = CreateApplication();
    app->initialize();

    // after it's done, shut down

    delete app;

    return 0;
}