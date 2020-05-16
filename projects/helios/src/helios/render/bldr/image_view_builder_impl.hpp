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
        uint32_t baseMip = 0;
        uint32_t mipLevels = 1;
        uint32_t baseArrayLayer = 0;
        uint32_t arrayLayers = 1;
    };
} // namespace helios
