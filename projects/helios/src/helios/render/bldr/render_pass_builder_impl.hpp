#pragma once

#include <helios/render/graphics.hpp>
#include <helios/vector.hpp>

namespace helios
{
    struct RenderPassBuilder::RenderPassBuilderImpl
    {
        IDevice* device = nullptr;
        vector<AttachmentDescription> attachments;
        vector<SubpassDescription> subpasses;
        vector<SubpassDependency> dependencies;
    };
} // namespace helios