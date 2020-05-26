#pragma once

#include <helios/render/graphics.hpp>

namespace helios
{
    struct DescriptorPoolBuilder::DescriptorPoolBuilderImpl
    {
        IDevice* device;
        u32 sets = 0;
        u32 samplerCount = 0;
        u32 imageSamplerCount = 0;
        u32 sampledImageCount = 0;
        u32 storageImageCount = 0;
        u32 uniformTexelCount = 0;
        u32 storageTexelCount = 0;
        u32 uniformCount = 0;
        u32 storageCount = 0;
        u32 uniformDynamicCount = 0;
        u32 storageDynamicCount = 0;
        u32 inputAttachmentCount = 0;
    };
} // namespace helios