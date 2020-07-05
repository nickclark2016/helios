#include <helios/render/vk/vk_physical_device.hpp>

#include <helios/render/vk/vk_context.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!context->destroyed)
            {
                this->context->devices.erase(
                    std::find(this->context->devices.begin(), this->context->devices.end(), this));
            }

            if (logicalDevice != nullptr && !logicalDevice->destroyed)
            {
                delete logicalDevice;
            }
        }
    }

    std::string VulkanPhysicalDevice::name() const
    {
        return deviceName;
    }

    EPhysicalDeviceType VulkanPhysicalDevice::type() const
    {
        return deviceType;
    }

    IPhysicalDevice::Features VulkanPhysicalDevice::features() const
    {
        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceFeatures(device, &feats);

        Features features = {feats.robustBufferAccess == VK_TRUE,
                             feats.fullDrawIndexUint32 == VK_TRUE,
                             feats.imageCubeArray == VK_TRUE,
                             feats.independentBlend == VK_TRUE,
                             feats.geometryShader == VK_TRUE,
                             feats.tessellationShader == VK_TRUE,
                             feats.sampleRateShading == VK_TRUE,
                             feats.dualSrcBlend == VK_TRUE,
                             feats.logicOp == VK_TRUE,
                             feats.multiDrawIndirect == VK_TRUE,
                             feats.drawIndirectFirstInstance == VK_TRUE,
                             feats.depthClamp == VK_TRUE,
                             feats.depthBiasClamp == VK_TRUE,
                             feats.fillModeNonSolid == VK_TRUE,
                             feats.depthBounds == VK_TRUE,
                             feats.wideLines == VK_TRUE,
                             feats.largePoints == VK_TRUE,
                             feats.alphaToOne == VK_TRUE,
                             feats.multiViewport == VK_TRUE,
                             feats.samplerAnisotropy == VK_TRUE,
                             feats.textureCompressionETC2 == VK_TRUE,
                             feats.textureCompressionASTC_LDR == VK_TRUE,
                             feats.textureCompressionBC == VK_TRUE,
                             feats.occlusionQueryPrecise == VK_TRUE,
                             feats.pipelineStatisticsQuery == VK_TRUE,
                             feats.vertexPipelineStoresAndAtomics == VK_TRUE,
                             feats.fragmentStoresAndAtomics == VK_TRUE,
                             feats.shaderTessellationAndGeometryPointSize == VK_TRUE,
                             feats.shaderImageGatherExtended == VK_TRUE,
                             feats.shaderStorageImageExtendedFormats == VK_TRUE,
                             feats.shaderStorageImageMultisample == VK_TRUE,
                             feats.shaderStorageImageReadWithoutFormat == VK_TRUE,
                             feats.shaderStorageImageWriteWithoutFormat == VK_TRUE,
                             feats.shaderUniformBufferArrayDynamicIndexing == VK_TRUE,
                             feats.shaderSampledImageArrayDynamicIndexing == VK_TRUE,
                             feats.shaderStorageBufferArrayDynamicIndexing == VK_TRUE,
                             feats.shaderStorageImageArrayDynamicIndexing == VK_TRUE,
                             feats.shaderClipDistance == VK_TRUE,
                             feats.shaderCullDistance == VK_TRUE,
                             feats.shaderFloat64 == VK_TRUE,
                             feats.shaderInt64 == VK_TRUE,
                             feats.shaderInt16 == VK_TRUE,
                             feats.shaderResourceResidency == VK_TRUE,
                             feats.shaderResourceMinLod == VK_TRUE,
                             feats.sparseBinding == VK_TRUE,
                             feats.sparseResidencyBuffer == VK_TRUE,
                             feats.sparseResidencyImage2D == VK_TRUE,
                             feats.sparseResidencyImage2D == VK_TRUE,
                             feats.sparseResidency2Samples == VK_TRUE,
                             feats.sparseResidency4Samples == VK_TRUE,
                             feats.sparseResidency8Samples == VK_TRUE,
                             feats.sparseResidency16Samples == VK_TRUE,
                             feats.sparseResidencyAliased == VK_TRUE,
                             feats.variableMultisampleRate == VK_TRUE,
                             feats.inheritedQueries == VK_TRUE};

        return features;
    }

    vector<IPhysicalDevice::QueueProperties> VulkanPhysicalDevice::queueProperties()
    {
        u32 propCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &propCount, nullptr);
        vector<VkQueueFamilyProperties> props(propCount);
        vector<QueueProperties> res;
        res.reserve(propCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &propCount, props.data());

        u32 index = 0;
        for (const auto& prop : props)
        {
            QueueProperties p = {index++,
                                 (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_FALSE,
                                 (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) != VK_FALSE,
                                 (prop.queueFlags & VK_QUEUE_TRANSFER_BIT) != VK_FALSE,
                                 (prop.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != VK_FALSE,
                                 (prop.queueFlags & VK_QUEUE_PROTECTED_BIT) != VK_FALSE,
                                 prop.queueCount};
            res.push_back(p);
        }

        return res;
    }
} // namespace helios