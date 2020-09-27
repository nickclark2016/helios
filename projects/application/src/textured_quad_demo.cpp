#include "textured_quad_demo.hpp"

#include "demo_utils.hpp"

#include <helios/core/engine_context.hpp>
#include <helios/core/window.hpp>
#include <helios/math/vector.hpp>
#include <helios/render/graphics.hpp>

#include <stb_image.h>

#include <fstream>
#include <iostream>
#include <string>

void textured_quad::run()
{
    using namespace helios;

    // clang-format off
    vector<f32> vertices = {
         0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
    };
    // clang-format on

    IWindow& window = EngineContext::instance().window();
    IDevice& device = EngineContext::instance().render().device();

    IQueue& graphicsQueue = EngineContext::instance().render().graphicsQueue();
    IQueue& transferQueue = EngineContext::instance().render().transferQueue();

    auto& swapchain = EngineContext::instance().render().swapchain();
    auto& presentQueue = EngineContext::instance().render().presentQueue();

    const auto views = swapchain.views();

    const auto vertexSource = read("assets/shaders/basic_texture/vert.spv");
    const auto fragmentSource = read("assets/shaders/basic_texture/frag.spv");

    const auto vertexModule =
        ShaderModuleBuilder().device(&device).source(vertexSource).build();
    const auto fragmentModule =
        ShaderModuleBuilder().device(&device).source(fragmentSource).build();

    auto setLayout = DescriptorSetLayoutBuilder()
                         .device(&device)
                         .bindings({
                             {0, EDescriptorType::UNIFORM_BUFFER, 1,
                              SHADER_STAGE_FRAGMENT_BIT},
                             {1, EDescriptorType::COMBINED_IMAGE_SAMPLER, 1,
                              SHADER_STAGE_FRAGMENT_BIT},
                         })
                         .build();

    const auto pipelineLayout =
        PipelineLayoutBuilder().device(&device).layouts({setLayout}).build();

    const auto renderpass =
        RenderPassBuilder()
            .device(&device)
            .attachments(
                {{swapchain.format(), SAMPLE_COUNT_1, EAttachmentLoadOp::CLEAR,
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
            .input({{{0, 5 * sizeof(f32), EVertexInputRate::VERTEX, 0,
                      EFormat::R32G32_SFLOAT, 0},
                     {0, 5 * sizeof(f32), EVertexInputRate::VERTEX, 1,
                      EFormat::R32G32_SFLOAT, 2 * sizeof(f32)}}})
            .assembly({EPrimitiveTopology::TRIANGLE_LIST, false})
            .tessellation({1})
            .viewports({{{0, static_cast<float>(window.height()),
                          static_cast<float>(window.width()),
                          -static_cast<float>(window.height()), 0.0f, 1.0f}},
                        {{0, 0, window.width(), window.height()}}})
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
    for (const auto& view : swapchain.views())
    {
        framebuffers.push_back(FramebufferBuilder()
                                   .attachments({view})
                                   .renderpass(renderpass)
                                   .width(window.width())
                                   .height(window.height())
                                   .layers(1)
                                   .build());
    }

    const auto commandPool =
        CommandPoolBuilder().device(&device).queue(&graphicsQueue).reset().build();

    const auto commandBuffers = commandPool->allocate(swapchain.imagesCount());
    vector<ISemaphore*> imageAvailable;
    vector<ISemaphore*> renderFinished;
    vector<IFence*> frameComplete;

    for (auto i = 0U; i < swapchain.imagesCount(); i++)
    {
        imageAvailable.push_back(SemaphoreBuilder().device(&device).build());
        renderFinished.push_back(SemaphoreBuilder().device(&device).build());
        frameComplete.push_back(
            FenceBuilder().device(&device).signaled().build());
    }

    auto stagingBuffer =
        BufferBuilder()
            .device(&device)
            .size(sizeof(float) * vertices.size())
            .usage(BUFFER_TYPE_VERTEX | BUFFER_TYPE_TRANSFER_SRC)
            .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE |
                           MEMORY_PROPERTY_HOST_COHERENT)
            .memoryUsage(EMemoryUsage::CPU_TO_GPU)
            .build();

    void* data = stagingBuffer->map();
    memcpy(data, vertices.data(), sizeof(float) * vertices.size());
    stagingBuffer->unmap();

    const auto vertexBuffer =
        BufferBuilder()
            .device(&device)
            .size(sizeof(float) * vertices.size())
            .usage(BUFFER_TYPE_VERTEX | BUFFER_TYPE_TRANSFER_DST)
            .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
            .memoryUsage(EMemoryUsage::GPU_ONLY)
            .build();

    // copy from staging to vertex buffer

    auto transferCmdPool =
        CommandPoolBuilder().device(&device).queue(&transferQueue).build();

    auto stagingFence = FenceBuilder().device(&device).build();
    auto stagingCmd = transferCmdPool->allocate();
    stagingCmd->record();
    stagingCmd->copy(stagingBuffer, vertexBuffer,
                     {{0, 0, sizeof(float) * vertices.size()}});
    stagingCmd->end();
    transferQueue.submit({{{}, {}, {}, {stagingCmd}}}, stagingFence);
    stagingFence->wait();

    i32 width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    void* pixels = stbi_load("assets/textures/dragon.png", &width, &height,
                             &channels, STBI_rgb_alpha);
    stbi_set_flip_vertically_on_load(false);

    delete stagingBuffer;
    stagingCmd = transferCmdPool->allocate();
    stagingBuffer = BufferBuilder()
                        .device(&device)
                        .size(sizeof(u8) * width * height * channels)
                        .usage(BUFFER_TYPE_TRANSFER_SRC)
                        .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE)
                        .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                        .build();
    data = stagingBuffer->map();
    memcpy(data, pixels, sizeof(u8) * width * height * channels);
    stagingBuffer->unmap();
    stbi_image_free(pixels);

    auto image = ImageBuilder()
                     .device(&device)
                     .type(EImageType::TYPE_2D)
                     .format(EFormat::R8G8B8A8_SRGB)
                     .extent(width, height, 1)
                     .mipLevels(1)
                     .arrayLayers(1)
                     .samples(SAMPLE_COUNT_1)
                     .tiling(EImageTiling::OPTIMAL)
                     .usage(IMAGE_TRANSFER_DST | IMAGE_SAMPLED)
                     .initialLayout(EImageLayout::UNDEFINED)
                     .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                     .memoryUsage(EMemoryUsage::GPU_ONLY)
                     .build();
    auto imageView = ImageViewBuilder()
                         .image(image)
                         .type(EImageViewType::TYPE_2D)
                         .format(EFormat::R8G8B8A8_SRGB)
                         .aspect(ASPECT_COLOR)
                         .build();
    auto sampler = SamplerBuilder()
                       .device(&device)
                       .anisotropy(16.0f)
                       .unnormalized(false)
                       .mipLodBias(0.0f)
                       .minLod(0.0f)
                       .maxLod(0.0f)
                       .build();

    stagingFence->reset();
    stagingCmd->record();
    stagingCmd->barrier(PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        PIPELINE_STAGE_TRANSFER_BIT, 0,
                        {{0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::UNDEFINED,
                          EImageLayout::TRANSFER_DST_OPTIMAL, ~(0U), ~(0U),
                          image, ASPECT_COLOR, 0, 1, 0, 1}});
    stagingCmd->copy(
        stagingBuffer, image,
        {{0, static_cast<u32>(width), static_cast<u32>(height), ASPECT_COLOR, 0,
          0, 1, 0, 0, 0, static_cast<u32>(width), static_cast<u32>(height), 1}},
        EImageLayout::TRANSFER_DST_OPTIMAL);
    stagingCmd->end();
    transferQueue.submit({{{}, {}, {}, {stagingCmd}}}, stagingFence);

    auto transitionCmd = commandPool->allocate();
    transitionCmd->record();
    stagingFence->wait();
    stagingFence->reset();
    transitionCmd->barrier(
        PIPELINE_STAGE_TOP_OF_PIPE_BIT, PIPELINE_STAGE_TRANSFER_BIT, 0,
        {{0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::TRANSFER_DST_OPTIMAL,
          EImageLayout::SHADER_READ_ONLY_OPTIMAL, ~(0U), ~(0U), image,
          ASPECT_COLOR, 0, 1, 0, 1}});
    transitionCmd->end();
    graphicsQueue.submit({{{}, {}, {}, {transitionCmd}}}, stagingFence);
    stagingFence->wait();

    delete stagingFence;
    delete stagingBuffer;
    delete stagingCmd;
    delete transferCmdPool;

    // Descriptor set pool
    auto descriptorPool = DescriptorPoolBuilder()
                              .device(&device)
                              .maxSetCount(swapchain.imagesCount())
                              .uniformBuffers(2)
                              .imageSamplers(2)
                              .build();

    auto sets = descriptorPool->allocate({setLayout, setLayout});

    auto colorBuffer = BufferBuilder()
                           .device(&device)
                           .usage(BUFFER_TYPE_UNIFORM)
                           .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                           .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE |
                                          MEMORY_PROPERTY_HOST_COHERENT)
                           .size(256)
                           .build();

    float colors[] = {1.0f, 1.0f, 0.0f};
    memcpy(colorBuffer->map(), colors, 3 * sizeof(f32));
    colorBuffer->unmap();

    for (const auto& set : sets)
    {
        DescriptorWriteInfo write = {};
        write.binding = 0;
        write.element = 0;
        write.type = EDescriptorType::UNIFORM_BUFFER;
        write.descriptorInfos =
            vector<DescriptorBufferInfo>({{colorBuffer, 0, 256}});
        set->write({write});

        write.binding = 1;
        write.element = 0;
        write.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
        write.descriptorInfos = vector<DescriptorImageInfo>(
            {{sampler, imageView, EImageLayout::SHADER_READ_ONLY_OPTIMAL}});
        set->write({write});
    }

    // record buffers
    for (auto i = 0U; i < swapchain.imagesCount(); i++)
    {
        commandBuffers[i]->record();
        commandBuffers[i]->beginRenderPass({renderpass,
                                            framebuffers[i],
                                            0,
                                            0,
                                            window.width(),
                                            window.height(),
                                            {{0.0f, 0.0f, 0.0f, 0.0f}}},
                                           true);
        commandBuffers[i]->bind({vertexBuffer}, {0}, 0);
        commandBuffers[i]->bind(pipeline);
        commandBuffers[i]->bind({sets[i]}, pipeline, 0);
        commandBuffers[i]->draw(6, 1, 0, 0);
        commandBuffers[i]->endRenderPass();
        commandBuffers[i]->end();
    }

    vector<IFence*> inFlightImages(frameComplete.size(), nullptr);

    size_t currentFrame = 0;
    while (!window.shouldClose())
    {
        const uint32_t imageIndex = swapchain.acquireNextImage(
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
        graphicsQueue.submit({submitInfo}, frameComplete[currentFrame]);
        presentQueue.present({{renderFinished[currentFrame]}, &swapchain, imageIndex});

        window.poll();

        currentFrame = (currentFrame + 1) % swapchain.imagesCount();

        if (window.getKeyboard().isPressed(EKey::KEY_ESCAPE))
        {
            break;
        }
    }

    device.idle();
}