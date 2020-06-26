#pragma once

#include <helios/containers/vector.hpp>
#include <helios/core/mesh.hpp>
#include <helios/render/graphics.hpp>

helios::EPresentMode get_best_present_mode(
    const helios::vector<helios::EPresentMode>& supported);

helios::ISurface::SurfaceFormat get_best_surface_format(
    const helios::vector<helios::ISurface::SurfaceFormat>& formats);

helios::vector<uint8_t> read(const std::string& filename);

void uploadMesh(helios::vector<helios::IBuffer*>& out,
                helios::IBuffer** elements, helios::IDevice* device,
                helios::IQueue* queue, helios::ICommandBuffer* commandBuffer,
                helios::Mesh* mesh);