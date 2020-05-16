#pragma once

#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <cstdint>

namespace helios
{
    struct SwapchainBuilder::SwapchainBuilderImpl
    {
        ISurface* surface = nullptr;
        uint32_t count = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t layers = 0;
        ISwapchain* previous = nullptr;
        EPresentMode presentMode = EPresentMode::IMMEDIATE;
        EFormat format = EFormat::UNDEFINED;
        EColorSpace colorSpace = EColorSpace::SRGB_NONLINEAR;
        vector<IQueue*> queues;
        EImageUsageFlags usage = 0;
        ESurfaceTransformFlagBits transform = TRANSFORM_IDENTITY;
        uint32_t alpha = 0;
        bool clipped = false;
    };
} // namespace helios