#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

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
        u32 computeCount = ~0U;
        u32 transferCount = ~0U;
        u32 graphicsCount = ~0U;
    };
} // namespace helios