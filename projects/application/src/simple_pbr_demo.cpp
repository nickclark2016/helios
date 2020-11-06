#include "simple_pbr_demo.hpp"

#include "demo_utils.hpp"

#include <helios/core/engine_context.hpp>
#include <helios/core/mesh.hpp>
#include <helios/core/window.hpp>
#include <helios/math/transformations.hpp>
#include <helios/math/vector.hpp>
#include <helios/render/graphics.hpp>
#include <helios/render/light.hpp>

#include <stb_image.h>

#include <fstream>
#include <iostream>
#include <string>

void simple_pbr::run()
{
    using namespace helios;

    IWindow& window = EngineContext::instance().window();
    IDevice& device = EngineContext::instance().render().device();

    IQueue& graphicsQueue = EngineContext::instance().render().graphicsQueue();
    IQueue& transferQueue = EngineContext::instance().render().transferQueue();

    auto& swapchain = EngineContext::instance().render().swapchain();
    auto& presentQueue = EngineContext::instance().render().presentQueue();

    const auto views = swapchain.views();

    const auto vertexSource = read("assets/shaders/pbr/vert.spv");
    const auto fragmentSource = read("assets/shaders/pbr/frag.spv");

    const auto vertexModule = ShaderModuleBuilder().device(&device).source(vertexSource).build();
    const auto fragmentModule = ShaderModuleBuilder().device(&device).source(fragmentSource).build();

    const auto depthImage = ImageBuilder()
                                .device(&device)
                                .type(EImageType::TYPE_2D)
                                .format(EFormat::D32_SFLOAT_S8_UINT)
                                .extent(window.width(), window.height(), 1)
                                .mipLevels(1)
                                .arrayLayers(1)
                                .samples(SAMPLE_COUNT_1)
                                .tiling(EImageTiling::OPTIMAL)
                                .usage(IMAGE_DEPTH_STENCIL_ATTACHMENT)
                                .initialLayout(EImageLayout::UNDEFINED)
                                .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                                .memoryUsage(EMemoryUsage::GPU_ONLY)
                                .build();
    const auto depthImageView = ImageViewBuilder()
                                    .type(EImageViewType::TYPE_2D)
                                    .image(depthImage)
                                    .format(EFormat::D32_SFLOAT_S8_UINT)
                                    .aspect(ASPECT_DEPTH)
                                    .baseMipLevel(0)
                                    .mipLevels(1)
                                    .baseArrayLayer(0)
                                    .arrayLayers(1)
                                    .build();

    auto renderStateSetLayout =
        DescriptorSetLayoutBuilder()
            .device(&device)
            .bindings({{0, EDescriptorType::UNIFORM_BUFFER, 1, SHADER_STAGE_VERTEX_BIT | SHADER_STAGE_FRAGMENT_BIT},
                       {1, EDescriptorType::UNIFORM_BUFFER, 1, SHADER_STAGE_FRAGMENT_BIT},
                       {2, EDescriptorType::UNIFORM_BUFFER, 1, SHADER_STAGE_FRAGMENT_BIT},
                       {3, EDescriptorType::COMBINED_IMAGE_SAMPLER, 3, SHADER_STAGE_FRAGMENT_BIT}})
            .build();

    auto modelStateSetLayout =
        DescriptorSetLayoutBuilder()
            .device(&device)
            .bindings({{0, EDescriptorType::UNIFORM_BUFFER, 1, SHADER_STAGE_VERTEX_BIT | SHADER_STAGE_FRAGMENT_BIT}})
            .build();

    const auto pipelineLayout =
        PipelineLayoutBuilder().device(&device).layouts({renderStateSetLayout, modelStateSetLayout}).build();

    const auto renderpass =
        RenderPassBuilder()
            .device(&device)
            .attachments({{swapchain.format(), SAMPLE_COUNT_1, EAttachmentLoadOp::CLEAR, EAttachmentStoreOp::STORE,
                           EAttachmentLoadOp::DONT_CARE, EAttachmentStoreOp::DONT_CARE, EImageLayout::UNDEFINED,
                           EImageLayout::PRESENT_SRC},
                          {EFormat::D32_SFLOAT_S8_UINT, SAMPLE_COUNT_1, EAttachmentLoadOp::CLEAR,
                           EAttachmentStoreOp::STORE, EAttachmentLoadOp::DONT_CARE, EAttachmentStoreOp::DONT_CARE,
                           EImageLayout::UNDEFINED, EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL}})
            .subpasses(
                {{EBindPoint::GRAPHICS,
                  {},
                  {{0, EImageLayout::COLOR_ATTACHMENT_OPTIMAL}},
                  {},
                  optional<RenderPassBuilder::AttachmentReference>({1, EImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL}),
                  {}}})
            .build();

    const auto pipeline =
        GraphicsPipelineBuilder()
            .vertex(vertexModule)
            .fragment(fragmentModule)
            .assembly({EPrimitiveTopology::TRIANGLE_LIST, false})
            .tessellation({1})
            .viewports({{{0, static_cast<float>(window.height()), static_cast<float>(window.width()),
                          -static_cast<float>(window.height()), 0.0f, 1.0f}},
                        {{0, 0, window.width(), window.height()}}})
            .rasterization({false, false, EPolygonMode::FILL, CULL_MODE_BACK, EVertexWindingOrder::COUNTER_CLOCKWISE,
                            false, 0.0f, 0.0f, 0.0f})
            .multisample({SAMPLE_COUNT_1, false, 0.0f, optional<uint32_t>(), false, false})
            .colorBlend({false,
                         ELogicOp::COPY,
                         {{false, EBlendFactor::SRC_COLOR, EBlendFactor::ONE_MINUS_CONSTANT_COLOR, EBlendOp::ADD,
                           EBlendFactor::SRC_ALPHA, EBlendFactor::ONE_MINUS_SRC_ALPHA, EBlendOp::ADD,
                           COLOR_COMPONENT_R | COLOR_COMPONENT_G | COLOR_COMPONENT_B | COLOR_COMPONENT_A}}})
            .depthStencil({true, true, ECompareOp::LESS, false, false, {}, {}, 0.0f, 1.0f})
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
                                   .attachments({view, depthImageView})
                                   .renderpass(renderpass)
                                   .width(window.width())
                                   .height(window.height())
                                   .layers(1)
                                   .build());
    }

    const auto commandPool = CommandPoolBuilder().device(&device).queue(&graphicsQueue).reset().build();

    const auto commandBuffers = commandPool->allocate(swapchain.imagesCount());
    vector<ISemaphore*> imageAvailable;
    vector<ISemaphore*> renderFinished;
    vector<IFence*> frameComplete;

    for (auto i = 0U; i < swapchain.imagesCount(); i++)
    {
        imageAvailable.push_back(SemaphoreBuilder().device(&device).build());
        renderFinished.push_back(SemaphoreBuilder().device(&device).build());
        frameComplete.push_back(FenceBuilder().device(&device).signaled().build());
    }

    auto transferCmdPool = CommandPoolBuilder().device(&device).queue(&transferQueue).build();
    auto stagingCmd = transferCmdPool->allocate();

    Mesh* mesh = new Mesh("assets/models/barramundi/BarramundiFish.gltf");
    vector<IBuffer*> buffers;
    IBuffer* elements;
    uploadMesh(buffers, &elements, &device, &transferQueue, stagingCmd, mesh->subMeshes[0]);

    i32 aWidth, aHeight, aChannels;
    i32 mWidth, mHeight, mChannels;
    i32 nWidth, nHeight, nChannels;
    stbi_set_flip_vertically_on_load(false);
    void* pixels = stbi_load("assets/models/barramundi/BarramundiFish_baseColor.png", &aWidth, &aHeight, &aChannels,
                             STBI_rgb_alpha);

    stagingCmd = transferCmdPool->allocate();
    auto stagingBuffer = BufferBuilder()
                             .device(&device)
                             .size(sizeof(u8) * aWidth * aHeight * STBI_rgb_alpha)
                             .usage(BUFFER_TYPE_TRANSFER_SRC)
                             .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE)
                             .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                             .build();
    void* data = stagingBuffer->map();
    memcpy(data, pixels, sizeof(u8) * aWidth * aHeight * STBI_rgb_alpha);
    stagingBuffer->unmap();
    stbi_image_free(pixels);

    auto albedo = ImageBuilder()
                      .device(&device)
                      .type(EImageType::TYPE_2D)
                      .format(EFormat::R8G8B8A8_SRGB)
                      .extent(aWidth, aHeight, 1)
                      .mipLevels(1)
                      .arrayLayers(1)
                      .samples(SAMPLE_COUNT_1)
                      .tiling(EImageTiling::OPTIMAL)
                      .usage(IMAGE_TRANSFER_DST | IMAGE_SAMPLED)
                      .initialLayout(EImageLayout::UNDEFINED)
                      .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                      .memoryUsage(EMemoryUsage::GPU_ONLY)
                      .build();
    auto albedoImageView = ImageViewBuilder()
                               .image(albedo)
                               .type(EImageViewType::TYPE_2D)
                               .format(EFormat::R8G8B8A8_SRGB)
                               .aspect(ASPECT_COLOR)
                               .build();

    pixels = stbi_load("assets/models/barramundi/"
                       "BarramundiFish_occlusionRoughnessMetallic.png",
                       &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);

    auto metallicStagingBuffer = BufferBuilder()
                                     .device(&device)
                                     .size(sizeof(u8) * mWidth * mHeight * STBI_rgb_alpha)
                                     .usage(BUFFER_TYPE_TRANSFER_SRC)
                                     .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE)
                                     .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                                     .build();
    data = metallicStagingBuffer->map();
    memcpy(data, pixels, sizeof(u8) * mWidth * mHeight * STBI_rgb_alpha);
    metallicStagingBuffer->unmap();
    stbi_image_free(pixels);

    auto metalRough = ImageBuilder()
                          .device(&device)
                          .type(EImageType::TYPE_2D)
                          .format(EFormat::R8G8B8A8_SRGB)
                          .extent(mWidth, mHeight, 1)
                          .mipLevels(1)
                          .arrayLayers(1)
                          .samples(SAMPLE_COUNT_1)
                          .tiling(EImageTiling::OPTIMAL)
                          .usage(IMAGE_TRANSFER_DST | IMAGE_SAMPLED)
                          .initialLayout(EImageLayout::UNDEFINED)
                          .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                          .memoryUsage(EMemoryUsage::GPU_ONLY)
                          .build();

    auto metalRoughImageView = ImageViewBuilder()
                                   .image(metalRough)
                                   .type(EImageViewType::TYPE_2D)
                                   .format(EFormat::R8G8B8A8_SRGB)
                                   .aspect(ASPECT_COLOR)
                                   .build();

    pixels =
        stbi_load("assets/models/barramundi/BarramundiFish_normal.png", &nWidth, &nHeight, &nChannels, STBI_rgb_alpha);

    auto normalStagingBuffer = BufferBuilder()
                                   .device(&device)
                                   .size(sizeof(u8) * nWidth * nHeight * STBI_rgb_alpha)
                                   .usage(BUFFER_TYPE_TRANSFER_SRC)
                                   .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE)
                                   .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                                   .build();
    data = normalStagingBuffer->map();
    memcpy(data, pixels, sizeof(u8) * nWidth * nHeight * STBI_rgb_alpha);
    normalStagingBuffer->unmap();
    stbi_image_free(pixels);

    auto normal = ImageBuilder()
                      .device(&device)
                      .type(EImageType::TYPE_2D)
                      .format(EFormat::R8G8B8A8_SRGB)
                      .extent(nWidth, nHeight, 1)
                      .mipLevels(1)
                      .arrayLayers(1)
                      .samples(SAMPLE_COUNT_1)
                      .tiling(EImageTiling::OPTIMAL)
                      .usage(IMAGE_TRANSFER_DST | IMAGE_SAMPLED)
                      .initialLayout(EImageLayout::UNDEFINED)
                      .requiredFlags(MEMORY_PROPERTY_DEVICE_LOCAL)
                      .memoryUsage(EMemoryUsage::GPU_ONLY)
                      .build();

    auto normalImageView = ImageViewBuilder()
                               .image(normal)
                               .type(EImageViewType::TYPE_2D)
                               .format(EFormat::R8G8B8A8_SRGB)
                               .aspect(ASPECT_COLOR)
                               .build();

    auto stagingFence = FenceBuilder().device(&device).build();
    stagingCmd->record();
    stagingCmd->barrier(PIPELINE_STAGE_TOP_OF_PIPE_BIT, PIPELINE_STAGE_TRANSFER_BIT, 0,
                        {},
                        {{0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::UNDEFINED, EImageLayout::TRANSFER_DST_OPTIMAL,
                          ~(0U), ~(0U), albedo, ASPECT_COLOR, 0, 1, 0, 1},
                         {0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::UNDEFINED, EImageLayout::TRANSFER_DST_OPTIMAL,
                          ~(0U), ~(0U), metalRough, ASPECT_COLOR, 0, 1, 0, 1},
                         {0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::UNDEFINED, EImageLayout::TRANSFER_DST_OPTIMAL,
                          ~(0U), ~(0U), normal, ASPECT_COLOR, 0, 1, 0, 1}});
    stagingCmd->copy(stagingBuffer, albedo,
                     {{0, static_cast<u32>(aWidth), static_cast<u32>(aHeight), ASPECT_COLOR, 0, 0, 1, 0, 0, 0,
                       static_cast<u32>(aWidth), static_cast<u32>(aHeight), 1}},
                     EImageLayout::TRANSFER_DST_OPTIMAL);

    stagingCmd->copy(metallicStagingBuffer, metalRough,
                     {{0, static_cast<u32>(mWidth), static_cast<u32>(mHeight), ASPECT_COLOR, 0, 0, 1, 0, 0, 0,
                       static_cast<u32>(mWidth), static_cast<u32>(mHeight), 1}},
                     EImageLayout::TRANSFER_DST_OPTIMAL);
    stagingCmd->copy(normalStagingBuffer, normal,
                     {{0, static_cast<u32>(nWidth), static_cast<u32>(nHeight), ASPECT_COLOR, 0, 0, 1, 0, 0, 0,
                       static_cast<u32>(nWidth), static_cast<u32>(nHeight), 1}},
                     EImageLayout::TRANSFER_DST_OPTIMAL);

    stagingCmd->end();
    transferQueue.submit({{{}, {}, {}, {stagingCmd}}}, stagingFence);

    auto sampler = SamplerBuilder()
                       .device(&device)
                       .anisotropy(1.0f)
                       .unnormalized(false)
                       .mipLodBias(0.0f)
                       .minLod(0.0f)
                       .maxLod(1.0f)
                       .mipmap(ESamplerMipMapMode::LINEAR)
                       .magnification(EFilter::LINEAR)
                       .minification(EFilter::LINEAR)
                       .build();

    auto transitionCmd = commandPool->allocate();
    transitionCmd->record();
    stagingFence->wait();
    stagingFence->reset();
    transitionCmd->barrier(
        PIPELINE_STAGE_TOP_OF_PIPE_BIT, PIPELINE_STAGE_TRANSFER_BIT, 0,
        {},
        {{0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::TRANSFER_DST_OPTIMAL, EImageLayout::SHADER_READ_ONLY_OPTIMAL,
          ~(0U), ~(0U), albedo, ASPECT_COLOR, 0, 1, 0, 1},
         {0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::TRANSFER_DST_OPTIMAL, EImageLayout::SHADER_READ_ONLY_OPTIMAL,
          ~(0U), ~(0U), metalRough, ASPECT_COLOR, 0, 1, 0, 1},
         {0, ACCESS_TRANSFER_WRITE_BIT, EImageLayout::TRANSFER_DST_OPTIMAL, EImageLayout::SHADER_READ_ONLY_OPTIMAL,
          ~(0U), ~(0U), normal, ASPECT_COLOR, 0, 1, 0, 1}});
    transitionCmd->end();
    graphicsQueue.submit({{{}, {}, {}, {transitionCmd}}}, stagingFence);
    stagingFence->wait();

    delete stagingFence;

    delete stagingCmd;
    delete transferCmdPool;

    // Descriptor set pool
    auto descriptorPool =
        DescriptorPoolBuilder().device(&device).maxSetCount(1024).uniformBuffers(8).imageSamplers(6).build();

    auto rendererSets = descriptorPool->allocate({renderStateSetLayout, renderStateSetLayout});

    auto modelSets = descriptorPool->allocate({modelStateSetLayout, modelStateSetLayout});

    auto rendererBuffer = BufferBuilder()
                              .device(&device)
                              .usage(BUFFER_TYPE_UNIFORM)
                              .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                              .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE | MEMORY_PROPERTY_HOST_COHERENT)
                              .size(256)
                              .build();

    auto lightingBuffer = BufferBuilder()
                              .device(&device)
                              .usage(BUFFER_TYPE_UNIFORM)
                              .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                              .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE | MEMORY_PROPERTY_HOST_COHERENT)
                              .size(256)
                              .build();

    auto materialBuffer = BufferBuilder()
                              .device(&device)
                              .usage(BUFFER_TYPE_UNIFORM)
                              .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                              .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE | MEMORY_PROPERTY_HOST_COHERENT)
                              .size(256)
                              .build();

    auto modelBuffer = BufferBuilder()
                           .device(&device)
                           .usage(BUFFER_TYPE_UNIFORM)
                           .memoryUsage(EMemoryUsage::CPU_TO_GPU)
                           .requiredFlags(MEMORY_PROPERTY_HOST_VISIBLE | MEMORY_PROPERTY_HOST_COHERENT)
                           .size(64 * 256)
                           .build();

    struct
    {
        Matrix4f proj;
        Matrix4f view;
        Vector3f cameraPos;
    } rendererData;

    rendererData.proj =
        perspective(90.0f, static_cast<float>(window.width()) / static_cast<float>(window.height()), 0.01f, 500.0f);
    rendererData.view = Matrix4f(1.0f);
    rendererData.cameraPos = Vector3f(0.0f);

    memcpy(rendererBuffer->map(), &rendererData, sizeof(rendererData));
    rendererBuffer->unmap();

    struct
    {
        float occlusion;
    } ao;

    ao.occlusion = 1.0f;
    memcpy(materialBuffer->map(), &ao, sizeof(ao));
    materialBuffer->unmap();

    struct
    {
        Matrix4f modl[256];
    } modelData;

    modelData.modl[0] = transform(Vector3f(0.0f, 0.0f, -3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(4.0f));
    memcpy(modelBuffer->map(), &modelData, sizeof(modelData));
    modelBuffer->unmap();

    for (const auto& set : rendererSets)
    {
        DescriptorWriteInfo write = {};
        write.binding = 0;
        write.element = 0;
        write.type = EDescriptorType::UNIFORM_BUFFER;
        write.descriptorInfos = vector<DescriptorBufferInfo>({{rendererBuffer, 0, 256}});
        set->write({write});

        write.binding = 3;
        write.element = 0;
        write.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
        write.descriptorInfos =
            vector<DescriptorImageInfo>({{sampler, albedoImageView, EImageLayout::SHADER_READ_ONLY_OPTIMAL}});
        set->write({write});

        write.binding = 3;
        write.element = 1;
        write.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
        write.descriptorInfos =
            vector<DescriptorImageInfo>({{sampler, normalImageView, EImageLayout::SHADER_READ_ONLY_OPTIMAL}});
        set->write({write});

        write.binding = 3;
        write.element = 2;
        write.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
        write.descriptorInfos =
            vector<DescriptorImageInfo>({{sampler, metalRoughImageView, EImageLayout::SHADER_READ_ONLY_OPTIMAL}});
        set->write({write});

        write.binding = 1;
        write.element = 0;
        write.type = EDescriptorType::UNIFORM_BUFFER;
        write.descriptorInfos = vector<DescriptorBufferInfo>({{lightingBuffer, 0, 256}});
        set->write({write});

        write.binding = 2;
        write.element = 0;
        write.type = EDescriptorType::UNIFORM_BUFFER;
        write.descriptorInfos = vector<DescriptorBufferInfo>({{materialBuffer, 0, 256}});
        set->write({write});
    }

    for (const auto& set : modelSets)
    {
        DescriptorWriteInfo write = {};
        write.binding = 0;
        write.element = 0;
        write.type = EDescriptorType::UNIFORM_BUFFER;
        write.descriptorInfos = vector<DescriptorBufferInfo>({{modelBuffer, 0, 64 * 256}});
        set->write({write});
    }

    vector<ClearValue> clear;
    clear.push_back({0.4f, 0.8f, 0.8f, 1.0f});
    ClearValue depthClear;
    depthClear.depthStencil = {1.0f, 0};
    clear.push_back(depthClear);

    // record buffers
    for (auto i = 0U; i < swapchain.imagesCount(); i++)
    {
        commandBuffers[i]->record();
        commandBuffers[i]->beginRenderPass({renderpass, framebuffers[i], 0, 0, window.width(), window.height(), clear},
                                           true);
        commandBuffers[i]->bind(buffers, {0, 0}, 0);
        commandBuffers[i]->bind(pipeline);
        commandBuffers[i]->bind(elements, 0);
        commandBuffers[i]->bind({rendererSets[i], modelSets[i]}, pipeline, 0);
        commandBuffers[i]->draw(static_cast<u32>(mesh->subMeshes[0]->triangles.size()), 1, 0, 0, 0);
        commandBuffers[i]->endRenderPass();
        commandBuffers[i]->end();
    }

    delete mesh;

    vector<IFence*> inFlightImages(frameComplete.size(), nullptr);

    float rotY = 90.0f;

    PointLight light;
    light.position = {0.0f, 15.0f, -3.0f};
    light.color = {1.0f, 1.0f, 1.0f};

    memcpy(lightingBuffer->map(), &light, sizeof(DirectionalLight));
    lightingBuffer->unmap();
    float delta = 1.0 / 60.0;
    float time = 0.0;

    size_t currentFrame = 0;
    while (!window.shouldClose())
    {
        modelData.modl[0] = transform(Vector3f(0.0f, -0.5f, -2.0f), Vector3f(0.0f, rotY, 0.0f), Vector3f(4.0f));
        memcpy(modelBuffer->map(), &modelData, sizeof(modelData));
        modelBuffer->unmap();

        light.position = {sinf(time) * 15.0f, 15.0f, cosf(time) * 15.0f};
        memcpy(lightingBuffer->map(), &light, sizeof(DirectionalLight));
        lightingBuffer->unmap();

        rotY += 0.1f;
        time += delta;

        const uint32_t imageIndex = swapchain.acquireNextImage(UINT64_MAX, imageAvailable[currentFrame], nullptr);
        if (inFlightImages[imageIndex] != nullptr)
        {
            inFlightImages[imageIndex]->wait();
        }
        inFlightImages[imageIndex] = frameComplete[currentFrame];

        IQueue::SubmitInfo submitInfo = {{imageAvailable[currentFrame]},
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