#pragma once

#include <helios/render/graphics.hpp>

namespace helios
{
    struct ImageBuilder::ImageBuilderImpl
    {
        IDevice* device;
        EImageType type;
        EFormat format;
        u32 width;
        u32 height;
        u32 depth;
        u32 mipLevelCount;
        u32 arrayLayerCount;
        ESampleCountFlagBits samples;
        EImageTiling tiling;
        EImageUsageFlags usage;
        vector<IQueue*> queues;
        EImageLayout layout;
        EMemoryPropertyFlags preferred;
        EMemoryPropertyFlags required;
        EMemoryUsage memUsage;
    };
} // namespace helios