#include "simple_render_graph.hpp"

#include "demo_utils.hpp"

#include <helios/core/mesh.hpp>
#include <helios/core/window.hpp>
#include <helios/math/transformations.hpp>
#include <helios/math/vector.hpp>
#include <helios/render/graphics.hpp>
#include <helios/render/light.hpp>
#include <helios/render/render_graph.hpp>

void simple_render_graph::run()
{
    using namespace helios;

    const auto ctx = ContextBuilder()
                         .applicationVersion(0, 0, 1)
                         .applicationName("Helios Sample")
                         .validation()
                         .build();

    const auto window =
        WindowBuilder().title("Helios Window").width(1024).height(1024).build();

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

    IQueue* graphicsQueue = nullptr;
    IQueue* presentQueue = nullptr;
    IQueue* transferQueue = nullptr;

    for (const auto& queue : device->queues())
    {
        if (queue->props().transfer)
        {
            transferQueue = queue;
        }
    }

    for (const auto& queue : device->queues())
    {
        if (queue->props().graphics)
        {
            graphicsQueue = queue;
        }
    }

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

    RenderGraph renderGraph(device, swapchain);
    renderGraph.addImage({"viewPositions",
                          ESizeType::SWAPCHAIN_RELATIVE,
                          {1.0f, 1.0f, 1.0f},
                          EImageType::TYPE_2D,
                          EFormat::R32G32B32A32_SFLOAT,
                          1,
                          1,
                          SAMPLE_COUNT_1,
                          EImageTiling::OPTIMAL,
                          IMAGE_COLOR_ATTACHMENT | IMAGE_INPUT_ATTACHMENT,
                          {}});
    renderGraph.addImage(
        {"depth",
         ESizeType::SWAPCHAIN_RELATIVE,
         {1.0f, 1.0f, 1.0f},
         EImageType::TYPE_2D,
         EFormat::D32_SFLOAT,
         1,
         1,
         SAMPLE_COUNT_1,
         EImageTiling::OPTIMAL,
         IMAGE_DEPTH_STENCIL_ATTACHMENT | IMAGE_INPUT_ATTACHMENT});
    renderGraph.addImage({"deferredComposite",
                          ESizeType::SWAPCHAIN_RELATIVE,
                          {1.0f, 1.0f, 1.0f},
                          EImageType::TYPE_2D,
                          EFormat::R8G8B8A8_SRGB,
                          1,
                          1,
                          SAMPLE_COUNT_1,
                          EImageTiling::OPTIMAL,
                          IMAGE_COLOR_ATTACHMENT});

    renderGraph.addAttachment(
        "viewPositions",
        {"viewPositions", ASPECT_COLOR, EImageViewType::TYPE_2D, 0, 1, 0, 1});
    renderGraph.addAttachment(
        "depth", {"depth", ASPECT_DEPTH, EImageViewType::TYPE_2D, 0, 1, 0, 1});
    renderGraph.addAttachment("deferredComposite",
                              {"deferredComposite", ASPECT_COLOR,
                               EImageViewType::TYPE_2D, 0, 1, 0, 1});

    RenderPass& deferredPass = renderGraph.addGraphicsPass("deferred");

    SubPass& geometrySubpass = deferredPass.addSubpass("geometry");
    geometrySubpass.addColorOutputAttachment("viewPositions");
    geometrySubpass.addDepthStencilAttachment("depth");

    SubPass& compositionSubpass = deferredPass.addSubpass("composite");
    compositionSubpass.addInputAttachment("viewPositions");
    compositionSubpass.addColorOutputAttachment("deferredComposite");

    compositionSubpass.dependsOn("geometry",
                                 {PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                  ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                  ACCESS_SHADER_READ_BIT, 0});

    deferredPass.setLayoutTransition("viewPositions", EImageLayout::UNDEFINED,
                                     EImageLayout::COLOR_ATTACHMENT_OPTIMAL);
    deferredPass.setLoadAction("viewPositions", EAttachmentLoadOp::CLEAR);
    deferredPass.setStoreAction("viewPositions", EAttachmentStoreOp::DONT_CARE);

    deferredPass.setLayoutTransition("deferredComposite",
                                     EImageLayout::UNDEFINED,
                                     EImageLayout::COLOR_ATTACHMENT_OPTIMAL);
    deferredPass.setLoadAction("deferredComposite", EAttachmentLoadOp::CLEAR);
    deferredPass.setStoreAction("deferredComposite", EAttachmentStoreOp::STORE);

    deferredPass.setLoadAction("depth", EAttachmentLoadOp::CLEAR);
    deferredPass.setStoreAction("depth", EAttachmentStoreOp::STORE);
    deferredPass.setLayoutTransition(
        "depth", EImageLayout::UNDEFINED,
        EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    renderGraph.build();

    while (!window->shouldClose())
    {
        window->poll();

        if (window->getKeyboard().isPressed(EKey::KEY_ESCAPE))
        {
            break;
        }
    }

    renderGraph.releaseResources();

    delete window;
    delete ctx;
}