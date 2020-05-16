#pragma once

#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <cstdint>
#include <string>

namespace helios
{
    struct DeviceBuilder::DeviceBuilderImpl
    {
        vector<QueueFamilyInfo> queues;
        vector<std::string> extensions;
        vector<std::string> layers;
        IPhysicalDevice::Features features = {};
        bool featuresSet = false;
        IPhysicalDevice* physicalDevice = nullptr;
        uint32_t computeCount = ~0U;
        uint32_t transferCount = ~0U;
        uint32_t graphicsCount = ~0U;
    };
} // namespace helios