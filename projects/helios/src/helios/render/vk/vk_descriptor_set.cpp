#include <helios/render/vk/vk_descriptor_set.hpp>

#include <helios/render/vk/vk_descriptor_pool.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    VulkanDescriptorSet::~VulkanDescriptorSet()
    {
        if (!destroyed)
        {
            destroyed = true;
            pool->sets.erase(
                std::find(pool->sets.begin(), pool->sets.end(), this));
            if (pool->sets.empty())
            {
                pool->reset();
            }
        }
    }
} // namespace helios