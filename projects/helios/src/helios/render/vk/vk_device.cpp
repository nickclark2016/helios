#include <helios/render/vk/vk_device.hpp>

#include <helios/render/bldr/device_builder_impl.hpp>
#include <helios/render/vk/vk_command_pool.hpp>
#include <helios/render/vk/vk_context.hpp>
#include <helios/render/vk/vk_descriptor_set_layout.hpp>
#include <helios/render/vk/vk_fence.hpp>
#include <helios/render/vk/vk_graphics_pipeline.hpp>
#include <helios/render/vk/vk_image.hpp>
#include <helios/render/vk/vk_physical_device.hpp>
#include <helios/render/vk/vk_pipeline_layout.hpp>
#include <helios/render/vk/vk_queue.hpp>
#include <helios/render/vk/vk_render_pass.hpp>
#include <helios/render/vk/vk_semaphore.hpp>
#include <helios/render/vk/vk_shader_module.hpp>
#include <helios/render/vk/vk_surface.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    static const std::string validation_layer = "VK_LAYER_KHRONOS_validation";

    static const std::string swapchain_extension =
        VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    static IPhysicalDevice::QueueProperties find_best_transfer(
        const vector<IPhysicalDevice::QueueProperties>& props)
    {
        // first search for dedicated
        for (const auto& fam : props)
        {
            if (fam.transfer && !fam.compute && !fam.graphics)
            {
                return fam;
            }
        }

        // next search for shared with one other
        for (const auto& fam : props)
        {
            if (fam.transfer && fam.compute && !fam.graphics)
            {
                return fam;
            }

            if (fam.transfer && !fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for generic with transfer
        for (const auto& fam : props)
        {
            if (fam.transfer)
            {
                return fam;
            }
        }

        return {~0U, false, false, false, false, false, 0};
    }

    static IPhysicalDevice::QueueProperties find_best_compute(
        const vector<IPhysicalDevice::QueueProperties>& props)
    {
        // first search for dedicated
        for (const auto& fam : props)
        {
            if (!fam.transfer && fam.compute && !fam.graphics)
            {
                return fam;
            }
        }

        // next search for shared with one other
        for (const auto& fam : props)
        {
            if (fam.transfer && fam.compute && !fam.graphics)
            {
                return fam;
            }

            if (!fam.transfer && fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for generic with compute
        for (const auto& fam : props)
        {
            if (fam.compute)
            {
                return fam;
            }
        }

        return {~0U, false, false, false, false, false, 0};
    }

    static IPhysicalDevice::QueueProperties find_best_graphics(
        const vector<IPhysicalDevice::QueueProperties>& props)
    {
        // first search for dedicated
        for (const auto& fam : props)
        {
            if (!fam.transfer && !fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for shared with one other
        for (const auto& fam : props)
        {
            if (!fam.transfer && fam.compute && fam.graphics)
            {
                return fam;
            }

            if (fam.transfer && !fam.compute && fam.graphics)
            {
                return fam;
            }
        }

        // next search for generic with graphics
        for (const auto& fam : props)
        {
            if (fam.graphics)
            {
                return fam;
            }
        }

        return {~0U, false, false, false, false, false, 0};
    }

    DeviceBuilder::DeviceBuilder()
    {
        _impl = new DeviceBuilderImpl;
    }

    DeviceBuilder::~DeviceBuilder()
    {
        delete _impl;
    }

    DeviceBuilder& DeviceBuilder::queues(const vector<QueueFamilyInfo>& infos)
    {
        _impl->queues = infos;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::features(
        const IPhysicalDevice::Features& features)
    {
        _impl->features = features;
        _impl->featuresSet = true;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::validation()
    {
        _impl->layers.push_back(validation_layer);
        return *this;
    }

    DeviceBuilder& DeviceBuilder::swapchain()
    {
        _impl->extensions.push_back(swapchain_extension);
        return *this;
    }

    DeviceBuilder& DeviceBuilder::physical(const IPhysicalDevice* pPhysical)
    {
        _impl->physicalDevice = const_cast<IPhysicalDevice*>(pPhysical);
        return *this;
    }

    DeviceBuilder& DeviceBuilder::compute(const u32 count)
    {
        _impl->computeCount = count;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::transfer(const u32 count)
    {
        _impl->transferCount = count;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::graphics(const u32 count)
    {
        _impl->graphicsCount = count;
        return *this;
    }

    IDevice* DeviceBuilder::build() const
    {
        VulkanDevice* device = new VulkanDevice;

        IPhysicalDevice* devPtr = _impl->physicalDevice;
        if (VulkanPhysicalDevice* vkDevPtr =
                dynamic_cast<VulkanPhysicalDevice*>(devPtr))
        {
            static bool init =
                gladLoaderLoadVulkan(vkDevPtr->context->instance,
                                     vkDevPtr->device, VK_NULL_HANDLE) > 0;
            if (!init)
            {
                exit(-1);
            }
        }

        VkPhysicalDeviceFeatures features;
        if (!_impl->featuresSet)
        {
            if (VulkanPhysicalDevice* vkDevPtr =
                    cast<VulkanPhysicalDevice*>(devPtr))
            {
                vkGetPhysicalDeviceFeatures(vkDevPtr->device, &features);
            }
        }
        else
        {
            IPhysicalDevice::Features& feats = _impl->features;
            features = {feats.robustBufferAccess,
                        feats.fullDrawUint32,
                        feats.imageCubeArray,
                        feats.independentBlend,
                        feats.geometryShader,
                        feats.tessellationShader,
                        feats.sampleRateShading,
                        feats.dualSourceBlend,
                        feats.logicOperations,
                        feats.multidrawIndirect,
                        feats.drawIndirectFirstInstance,
                        feats.depthClamp,
                        feats.depthBiasClamp,
                        feats.fillModeNonSolid,
                        feats.depthBounds,
                        feats.wideLines,
                        feats.largePoints,
                        feats.alphaToOne,
                        feats.multiViewport,
                        feats.samplerAnisotropy,
                        feats.textureCompressionEtc2,
                        feats.textureCompressionAstcLdr,
                        feats.textureCompressionBc,
                        feats.occlusionQueryPrecise,
                        feats.pipelineStatisticsQuery,
                        feats.vertexPipelineStoresAndAtomics,
                        feats.fragmentStoresAndAtomics,
                        feats.shaderTessellationAndGeometryPointSize,
                        feats.shaderImageGatherExtended,
                        feats.shaderStorageImageExtendedFormats,
                        feats.shaderStorageImageMultisample,
                        feats.shaderStorageImageReadWithoutFormat,
                        feats.shaderStorageImageWriteWithoutFormat,
                        feats.shaderUniformBufferArrayDynamicIndexing,
                        feats.shaderSampledImageArrayDynamicIndexing,
                        feats.shaderStorageBufferArrayDynamicIndexing,
                        feats.shaderStorageImageArrayDynamicIndexing,
                        feats.shaderClipDistance,
                        feats.shaderCullDistance,
                        feats.shaderFloat64,
                        feats.shaderInt64,
                        feats.shaderInt16,
                        feats.shaderResourceResidency,
                        feats.shaderResourceMinLod,
                        feats.sparseBinding,
                        feats.sparseResidencyBuffer,
                        feats.sparseResidencyImage2D,
                        feats.sparseResidencyImage3D,
                        feats.sparseResidency2Samples,
                        feats.sparseResidency4Samples,
                        feats.sparseResidency8Samples,
                        feats.sparseResidency16Samples,
                        feats.sparseResidencyAliased,
                        feats.variableMultisampleRate,
                        feats.inheritedQueries};
        }

        vector<VkDeviceQueueCreateInfo> deviceQueueInfos;
        vector<vector<f32>> queuePris;
        deviceQueueInfos.reserve(_impl->queues.size());
        queuePris.reserve(_impl->queues.size());

        if (_impl->queues.empty())
        {
            const auto queues = _impl->physicalDevice->queueProperties();

            const auto transfer = find_best_transfer(queues);
            const auto compute = find_best_compute(queues);
            const auto graphics = find_best_graphics(queues);

            if (_impl->transferCount == ~0U)
            {
                _impl->transferCount = 1;
            }

            if (_impl->computeCount == ~0U)
            {
                _impl->computeCount = 1;
            }

            if (_impl->graphicsCount == ~0U)
            {
                _impl->graphicsCount = 1;
            }

            if (_impl->transferCount > 0)
            {
                _impl->queues.push_back(
                    {transfer.index, false,
                     vector<f32>(_impl->transferCount,
                                 1.0f /
                                     static_cast<f32>(_impl->transferCount))});
            }

            if (_impl->computeCount > 0)
            {
                _impl->queues.push_back(
                    {compute.index, false,
                     vector<f32>(_impl->computeCount,
                                 1.0f /
                                     static_cast<f32>(_impl->computeCount))});
            }

            if (_impl->graphicsCount > 0)
            {
                _impl->queues.push_back(
                    {graphics.index, false,
                     vector<f32>(_impl->graphicsCount,
                                 1.0f /
                                     static_cast<f32>(_impl->graphicsCount))});
            }
        }

        std::sort(_impl->queues.begin(), _impl->queues.end(),
                  [](QueueFamilyInfo& left, QueueFamilyInfo& right) {
                      return left.family < right.family;
                  });

        for (const auto& info : _impl->queues)
        {
            VkDeviceQueueCreateInfo queue = {};
            queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue.queueFamilyIndex = info.family;
            queue.flags |=
                info.protect ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0;
            queuePris.push_back(info.priorities);
            queue.pQueuePriorities = queuePris[queuePris.size() - 1].data();
            queue.queueCount = static_cast<u32>(info.priorities.size());
            deviceQueueInfos.push_back(queue);
        }

        vector<const char*> layers;
        vector<const char*> extensions;

        for (const auto& layer : _impl->layers)
        {
            layers.push_back(layer.c_str());
        }

        for (const auto& ext : _impl->extensions)
        {
            extensions.push_back(ext.c_str());
        }

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.enabledExtensionCount = static_cast<u32>(extensions.size());
        info.enabledLayerCount = static_cast<u32>(layers.size());
        info.pEnabledFeatures = &features;
        info.ppEnabledExtensionNames = extensions.data();
        info.ppEnabledLayerNames = layers.data();
        info.pQueueCreateInfos = deviceQueueInfos.data();
        info.queueCreateInfoCount = static_cast<u32>(deviceQueueInfos.size());

        if (VulkanPhysicalDevice* vkDevPtr =
                dynamic_cast<VulkanPhysicalDevice*>(devPtr))
        {
            if (vkCreateDevice(vkDevPtr->device, &info, nullptr,
                               &device->device) != VK_SUCCESS)
            {
                device = nullptr;
            }

            static bool init =
                gladLoaderLoadVulkan(vkDevPtr->context->instance,
                                     vkDevPtr->device, device->device) > 0;
            if (!init)
            {
                exit(-1);
            }

            const auto queues = _impl->physicalDevice->queueProperties();
            if (!_impl->queues.empty())
            {
                for (const auto& queueInfo : _impl->queues)
                {
                    const auto idx = queueInfo.family;
                    for (const auto prop : queues)
                    {
                        if (prop.index == idx)
                        {
                            for (u32 i = 0;
                                 i <
                                 static_cast<u32>(queueInfo.priorities.size());
                                 i++)
                            {
                                VulkanQueue* queue = new VulkanQueue;
                                queue->family = prop;
                                queue->queueIndex = i;
                                queue->queuePriority = queueInfo.priorities[i];
                                vkGetDeviceQueue(
                                    device->device, queue->family.index,
                                    queue->queueIndex, &queue->queue);
                                device->deviceQueues.push_back(queue);
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                // No queues specified, populate all queues
                for (const auto prop : queues)
                {
                    for (u32 i = 0; i < prop.count; i++)
                    {
                        VulkanQueue* queue = new VulkanQueue;
                        queue->family = prop;
                        queue->queueIndex = i;
                        queue->queuePriority =
                            1.0f / static_cast<f32>(prop.count);
                        vkGetDeviceQueue(device->device, queue->family.index,
                                         queue->queueIndex, &queue->queue);
                        device->deviceQueues.push_back(queue);
                    }
                }
            }
        }

        device->parent = cast<VulkanPhysicalDevice*>(_impl->physicalDevice);
        device->parent->logicalDevice = device;
        return device;
    }

    VulkanDevice::~VulkanDevice()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (const auto& fence : fences)
            {
                delete fence;
            }

            for (const auto& sem : sems)
            {
                delete sem;
            }

            for (const auto& pool : commandBufferPools)
            {
                delete pool;
            }

            for (const auto& gp : graphicsPipelines)
            {
                delete gp;
            }

            for (const auto& layout : pipelineLayouts)
            {
                delete layout;
            }

            for (const auto& pass : renderPasses)
            {
                delete pass;
            }

            for (const auto& layout : setLayouts)
            {
                delete layout;
            }

            for (const auto& mod : modules)
            {
                delete mod;
            }

            for (const auto& image : images)
            {
                delete image;
            }

            for (const auto& queue : deviceQueues)
            {
                delete queue;
            }

            for (const auto& surface : surfaces)
            {
                delete surface;
            }

            if (!parent->destroyed)
            {
                parent->device = nullptr;
                delete parent;
            }

            vkDestroyDevice(device, nullptr);
        }
    }

    vector<IQueue*> VulkanDevice::queues() const
    {
        return deviceQueues;
    }

    void VulkanDevice::idle() const
    {
        vkDeviceWaitIdle(device);
    }
} // namespace helios