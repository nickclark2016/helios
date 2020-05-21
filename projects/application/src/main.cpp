#include <helios/core/window.hpp>
#include <helios/render/graphics.hpp>

#include <fstream>
#include <iostream>
#include <string>

helios::EPresentMode get_best_present_mode(
    const helios::vector<helios::EPresentMode>& supported)
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

helios::ISurface::SurfaceFormat get_best_surface_format(
    const helios::vector<helios::ISurface::SurfaceFormat>& formats)
{
    for (const auto& format : formats)
    {
        if (format.format == helios::EFormat::B8G8R8A8_SRGB &&
            format.colorSpace == helios::EColorSpace::SRGB_NONLINEAR)
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

int main()
{
    using namespace helios;

    dynamic_block_allocator<u32, 1024, sizeof(u32)> allocator;
    u32* ptr = allocator.allocate(32);
    allocator.release(ptr);

    const auto ctx = ContextBuilder()
                         .applicationVersion(0, 0, 1)
                         .applicationName("Helios Sample")
                         .validation()
                         .build();

    const auto window =
        WindowBuilder().title("Helios Window").width(1280).height(720).build();

    const auto physicalDevices = ctx->physicalDevices();

    const auto physicalDevice = physicalDevices[0];
    const auto device = DeviceBuilder()
                            .validation()
                            .physical(physicalDevice)
                            .transfer(1)
                            .compute(1)
                            .graphics(2)
                            .validation()
                            .swapchain()
                            .build();

    const auto surface = SurfaceBuilder().device(device).window(window).build();

    IQueue* presentQueue = nullptr;

    for (const auto& queue : device->queues())
    {
        if (queue->canPresent(physicalDevice, surface))
        {
            presentQueue = queue;
            break;
        }
    }

    const auto swapchainSupport = surface->swapchainSupport(physicalDevice);
    const auto swapchain =
        SwapchainBuilder()
            .surface(surface)
            .images(2)
            .width(window->width())
            .height(window->height())
            .layers(1)
            .present(get_best_present_mode(swapchainSupport.presentModes))
            .format(
                get_best_surface_format(swapchainSupport.surfaceFormats).format)
            .colorSpace(get_best_surface_format(swapchainSupport.surfaceFormats)
                            .colorSpace)
            .queues({presentQueue})
            .usage(IMAGE_COLOR_ATTACHMENT)
            .transform(swapchainSupport.currentTransform)
            .alphaOpaque()
            .clipped()
            .build();

    const auto views = swapchain->views();

    const auto vertexSource = read("res/shaders/basic/vert.spv");
    const auto fragmentSource = read("res/shaders/basic/frag.spv");

    const auto vertexModule =
        ShaderModuleBuilder().device(device).source(vertexSource).build();
    const auto fragmentModule =
        ShaderModuleBuilder().device(device).source(fragmentSource).build();

    const auto pipelineLayout = PipelineLayoutBuilder().device(device).build();

    const auto renderpass =
        RenderPassBuilder()
            .device(device)
            .attachments(
                {{swapchain->format(), SAMPLE_COUNT_1, EAttachmentLoadOp::CLEAR,
                  EAttachmentStoreOp::STORE, EAttachmentLoadOp::DONT_CARE,
                  EAttachmentStoreOp::DONT_CARE, EImageLayout::UNDEFINED,
                  EImageLayout::PRESENT_SRC}})
            .subpasses({{EBindPoint::GRAPHICS,
                         {},
                         {{0, EImageLayout::COLOR_ATTACHMENT_OPTIMAL}},
                         {},
                         optional<RenderPassBuilder::AttachmentReference>(),
                         {}}})
            .build();

    const auto pipeline =
        GraphicsPipelineBuilder()
            .vertex(vertexModule)
            .fragment(fragmentModule)
            .input({})
            .assembly({EPrimitiveTopology::TRIANGLE_LIST, false})
            .tessellation({1})
            .viewports({{{0, 0, static_cast<float>(window->width()),
                          static_cast<float>(window->height()), 0.0f, 1.0f}},
                        {{0, 0, window->width(), window->height()}}})
            .rasterization({false, false, EPolygonMode::FILL, CULL_MODE_BACK,
                            EVertexWindingOrder::CLOCKWISE, false, 0.0f, 0.0f,
                            0.0f})
            .multisample({SAMPLE_COUNT_1, false, 0.0f, optional<uint32_t>(),
                          false, false})
            .colorBlend({false,
                         ELogicOp::COPY,
                         {{false, EBlendFactor::SRC_COLOR,
                           EBlendFactor::ONE_MINUS_CONSTANT_COLOR,
                           EBlendOp::ADD, EBlendFactor::SRC_ALPHA,
                           EBlendFactor::ONE_MINUS_SRC_ALPHA, EBlendOp::ADD,
                           COLOR_COMPONENT_R | COLOR_COMPONENT_G |
                               COLOR_COMPONENT_B | COLOR_COMPONENT_A}}})
            .dynamic({{

            }})
            .layout(pipelineLayout)
            .subpass(0)
            .renderpass(renderpass)
            .build();

    vector<IFramebuffer*> framebuffers;
    for (const auto& view : swapchain->views())
    {
        framebuffers.push_back(FramebufferBuilder()
                                   .attachments({view})
                                   .renderpass(renderpass)
                                   .width(window->width())
                                   .height(window->height())
                                   .layers(1)
                                   .build());
    }

    const auto commandPool =
        CommandPoolBuilder().device(device).queue(presentQueue).reset().build();

    const auto commandBuffers = commandPool->allocate(swapchain->imagesCount());
    vector<ISemaphore*> imageAvailable;
    vector<ISemaphore*> renderFinished;
    vector<IFence*> frameComplete;

    for (auto i = 0U; i < swapchain->imagesCount(); i++)
    {
        imageAvailable.push_back(SemaphoreBuilder().device(device).build());
        renderFinished.push_back(SemaphoreBuilder().device(device).build());
        frameComplete.push_back(
            FenceBuilder().device(device).signaled().build());
    }

    // record buffers
    for (auto i = 0U; i < swapchain->imagesCount(); i++)
    {
        commandBuffers[i]->record();
        commandBuffers[i]->beginRenderPass({renderpass,
                                            framebuffers[i],
                                            0,
                                            0,
                                            window->width(),
                                            window->height(),
                                            {{0.0f, 0.0f, 0.0f, 0.0f}}},
                                           true);
        commandBuffers[i]->bind(pipeline);
        commandBuffers[i]->draw(3, 1, 0, 0);
        commandBuffers[i]->endRenderPass();
        commandBuffers[i]->end();
    }

    vector<IFence*> inFlightImages(frameComplete.size(), nullptr);

    size_t currentFrame = 0;
    while (!window->shouldClose())
    {
        const uint32_t imageIndex = swapchain->acquireNextImage(
            UINT64_MAX, imageAvailable[currentFrame], nullptr);
        if (inFlightImages[imageIndex] != nullptr)
        {
            inFlightImages[imageIndex]->wait();
        }
        inFlightImages[imageIndex] = frameComplete[currentFrame];

        IQueue::SubmitInfo submitInfo = {
            {imageAvailable[currentFrame]},
            {PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
            {renderFinished[currentFrame]},
            {commandBuffers[currentFrame]}};

        frameComplete[currentFrame]->reset();
        presentQueue->submit({submitInfo}, frameComplete[currentFrame]);
        presentQueue->present(
            {{renderFinished[currentFrame]}, swapchain, imageIndex});

        window->poll();

        currentFrame = (currentFrame + 1) % swapchain->imagesCount();
    }

    device->idle();

    delete window;
    delete ctx;

    return 0;
}