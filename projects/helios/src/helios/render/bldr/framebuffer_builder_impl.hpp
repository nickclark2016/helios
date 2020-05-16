#pragma once

#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

#include <cstdint>

namespace helios
{
    struct FramebufferBuilder::FramebufferBuilderImpl
    {
        vector<IImageView*> views;
        IRenderPass* renderpass = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t layers = 1;
    };
} // namespace helios