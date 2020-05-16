#!/bin/sh

# Download GLAD
rm -rf ./dependencies/glad/include
glad --out-path=./dependencies/glad --api=vulkan --extensions="VK_KHR_surface,VK_KHR_swapchain,VK_EXT_debug_utils" c --loader --header