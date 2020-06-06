#pragma once

#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

#include <cstdint>

namespace helios
{
    struct ImageViewBuilder::ImageViewBuilderImpl
    {
        EImageViewType type = EImageViewType::TYPE_2D;
        IImage* image = nullptr;
        EFormat format = EFormat::UNDEFINED;
        EComponentSwizzle red = EComponentSwizzle::IDENTITY;
        EComponentSwizzle green = EComponentSwizzle::IDENTITY;
        EComponentSwizzle blue = EComponentSwizzle::IDENTITY;
        EComponentSwizzle alpha = EComponentSwizzle::IDENTITY;
        EImageAspectFlags aspect = 0;
        u32 baseMip = 0;
        u32 mipLevels = 1;
        u32 baseArrayLayer = 0;
        u32 arrayLayers = 1;
    };
} // namespace helios
