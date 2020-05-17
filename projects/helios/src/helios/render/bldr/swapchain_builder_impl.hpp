#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

#include <cstdint>

namespace helios
{
    struct SwapchainBuilder::SwapchainBuilderImpl
    {
        ISurface* surface = nullptr;
        u32 count = 0;
        u32 width = 0;
        u32 height = 0;
        u32 layers = 0;
        ISwapchain* previous = nullptr;
        EPresentMode presentMode = EPresentMode::IMMEDIATE;
        EFormat format = EFormat::UNDEFINED;
        EColorSpace colorSpace = EColorSpace::SRGB_NONLINEAR;
        vector<IQueue*> queues;
        EImageUsageFlags usage = 0;
        ESurfaceTransformFlagBits transform = TRANSFORM_IDENTITY;
        u32 alpha = 0;
        bool clipped = false;
    };
} // namespace helios