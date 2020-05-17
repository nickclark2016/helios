#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

#include <cstdint>

namespace helios
{
    struct FramebufferBuilder::FramebufferBuilderImpl
    {
        vector<IImageView*> views;
        IRenderPass* renderpass = nullptr;
        u32 width = 0;
        u32 height = 0;
        u32 layers = 1;
    };
} // namespace helios