#pragma once

#include <helios/render/enums.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    struct SamplerBuilder::SamplerBuilderImpl
    {
        IDevice* device = nullptr;
        EFilter magnification = EFilter::NEAREST;
        EFilter minification = EFilter::NEAREST;
        ESamplerMipMapMode mip = ESamplerMipMapMode::NEAREST;
        ESamplerAddressMode addrU = ESamplerAddressMode::REPEAT;
        ESamplerAddressMode addrV = ESamplerAddressMode::REPEAT;
        ESamplerAddressMode addrW = ESamplerAddressMode::REPEAT;
        f32 mipLodBias = 0.0f;
        bool anisoEnable = false;
        f32 maxAniso = 0.0f;
        bool compareEnable = false;
        ECompareOp compare = ECompareOp::NEVER;
        f32 minLod = 0.0f;
        f32 maxLod = 0.0f;
        EBorderColor border = EBorderColor::FLOAT_OPAQUE_BLACK;
        bool unnormalized;
    };
} // namespace helios
