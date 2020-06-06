#pragma once

#include <helios/containers/vector.hpp>
#include <helios/render/graphics.hpp>

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