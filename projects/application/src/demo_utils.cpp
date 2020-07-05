#include "demo_utils.hpp"

#include <helios/core/mesh.hpp>
#include <helios/render/graphics.hpp>

#include <fstream>
#include <iostream>

helios::EPresentMode get_best_present_mode(const helios::vector<helios::EPresentMode>& supported)
{
    for (const auto mode : supported)
    {
        if (mode == helios::EPresentMode::MAILBOX)
        {
            return mode;
        }
    }
    return helios::EPresentMode::FIFO;
}

helios::ISurface::SurfaceFormat get_best_surface_format(const helios::vector<helios::ISurface::SurfaceFormat>& formats)
{
    for (const auto& format : formats)
    {
        if (format.format == helios::EFormat::B8G8R8A8_SRGB && format.colorSpace == helios::EColorSpace::SRGB_NONLINEAR)
        {
            return format;
        }
    }
    return formats[0];
}

helios::vector<uint8_t> read(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    helios::vector<uint8_t> res;
    if (file.is_open())
    {
        const size_t sz = static_cast<size_t>(file.tellg());
        res.resize(sz);
        file.seekg(0);
        file.read(reinterpret_cast<char*>(res.data()), sz);
        file.close();
    }
    return res;
}

void uploadMesh(helios::vector<helios::IBuffer*>& out, helios::IBuffer** elements, helios::IDevice* device,
                helios::IQueue* queue, helios::ICommandBuffer* commandBuffer, helios::Mesh* mesh)
{
    using namespace helios;

    commandBuffer->record();
    auto stagingFence = FenceBuilder().device(device).build();
    stagingFence->reset();
    out.reserve(mesh->bufferCount());
    u32 count = mesh->bufferCount();

    vector<IBuffer*> staging;

    for (u32 i = 0; i < count; i++)
    {
        void* data = mesh->readBuffer(i);
        u64 size = mesh->bufferSize(i);

        auto stagingBuffer = BufferBuilder()
                                 .device(device)
                                 .size(size)
                                 .usage(BUFFER_TYPE_TRANSFER_SRC)
                                 .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE)
                                 .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                                 .build();
        staging.push_back(stagingBuffer);
        void* payload = stagingBuffer->map();
        memcpy(payload, data, size);
        stagingBuffer->unmap();

        auto resultBuffer = BufferBuilder()
                                .device(device)
                                .size(size)
                                .usage(BUFFER_TYPE_VERTEX | BUFFER_TYPE_TRANSFER_DST)
                                .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                                .memoryUsage(EMemoryUsage::GPU_ONLY)
                                .build();

        commandBuffer->copy(stagingBuffer, resultBuffer, {{0, 0, size}});

        out.push_back(resultBuffer);
    }

    if (!mesh->triangles.empty())
    {
        u64 size = sizeof(u32) * mesh->triangles.size();
        auto stagingBuffer = BufferBuilder()
                                 .device(device)
                                 .size(size)
                                 .usage(BUFFER_TYPE_TRANSFER_SRC)
                                 .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE)
                                 .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                                 .build();
        staging.push_back(stagingBuffer);
        void* payload = stagingBuffer->map();
        memcpy(payload, mesh->triangles.data(), size);
        stagingBuffer->unmap();

        auto resultBuffer = BufferBuilder()
                                .device(device)
                                .size(size)
                                .usage(BUFFER_TYPE_INDEX | BUFFER_TYPE_TRANSFER_DST)
                                .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                                .memoryUsage(EMemoryUsage::GPU_ONLY)
                                .build();

        commandBuffer->copy(stagingBuffer, resultBuffer, {{0, 0, size}});

        *elements = resultBuffer;
    }

    commandBuffer->end();
    queue->submit({{{}, {}, {}, {commandBuffer}}}, stagingFence);
    stagingFence->wait();

    staging.clear();
}