#!/bin/sh

# Clean all previous generated project files
find . -name "*.vcxproj*" -type f -delete
find . -name "*.sln*" -type f -delete
find . -name "Makefile" -type f -delete

# Clean all previous generated files
rm -rf bin/
rm -rf bin-int/

# Download GLAD Vulkan Headers
rm -rf ./dependencies/glad/include
py=python
command -v python3 >/dev/null 2>&1 && py=python3
exec $py -m glad --out-path=./dependencies/glad --api=vulkan --extensions="VK_KHR_surface,VK_KHR_swapchain,\
    VK_EXT_debug_utils,VK_KHR_get_memory_requirements2,VK_KHR_get_physical_device_properties2,\
    VK_KHR_bind_memory2,VK_EXT_debug_report,VK_KHR_dedicated_allocation,VK_KHR_device_group,VK_KHR_buffer_device_address,\
    VK_EXT_buffer_device_address" c --loader --header
