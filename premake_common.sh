#!/bin/sh

# Download GLAD
rm -rf ./dependencies/glad/include
~/.local/bin/glad --out-path=./dependencies/glad --api=vulkan --extensions="VK_KHR_surface,VK_KHR_swapchain,\
    VK_EXT_debug_utils,VK_KHR_get_memory_requirements2,VK_KHR_get_physical_device_properties2,\
    VK_KHR_bind_memory2,VK_EXT_debug_report,VK_KHR_dedicated_allocation,VK_KHR_device_group,VK_KHR_buffer_device_address,\
    VK_EXT_buffer_device_address" c --loader --header