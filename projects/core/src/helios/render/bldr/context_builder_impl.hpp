#pragma once

#include <helios/render/graphics.hpp>

#include <cstdint>
#include <functional>
#include <string>

#define HELIOS_VERSION_MAJOR 0
#define HELIOS_VERSION_MINOR 0
#define HELIOS_VERSION_PATCH 1

namespace helios
{
    struct ContextBuilder::ContextBuilderImpl
    {
        std::string appName;
        u32 appVersion = VK_MAKE_VERSION(0, 0, 0);
        std::string engineName = "Helios";
        u32 engineVersion = VK_MAKE_VERSION(HELIOS_VERSION_MAJOR, HELIOS_VERSION_MINOR, HELIOS_VERSION_PATCH);
        vector<std::string> extensions;
        vector<std::string> layers;
        std::function<void(EMessageSeverity, const char*)> debugCallback;
    };
} // namespace helios