#pragma once

#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    struct BufferBuilder::BufferBuilderImpl
    {
        IDevice* device = nullptr;
        u64 size = 0;
        vector<IQueue*> queues;
        EBufferTypeFlags usage = 0;
        EMemoryPropertyFlags preferred = 0;
        EMemoryPropertyFlags required = 0;
        EMemoryUsage memoryUsage = EMemoryUsage::CPU_ONLY;
    };
} // namespace helios