#include <helios/render/vk/vk_context.hpp>

#include <helios/render/bldr/context_builder_impl.hpp>
#include <helios/render/vk/vk_physical_device.hpp>

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

#include <string>

namespace helios
{
    static const std::string validation_layer = "VK_LAYER_KHRONOS_validation";
    static const std::string debug_extension =
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    static const std::string swapchain_extension =
        VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    std::function<void(EMessageSeverity, const std::string)>
        ContextBuilder::DefaultCallback =
            [](EMessageSeverity severity, const std::string& message) {
                if (severity == EMessageSeverity::WARNING ||
                    severity == EMessageSeverity::ERROR)
                {
                    fprintf(stderr, "%s\n", message.c_str());
                }
            };

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  const VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData)
    {

        std::function<void(EMessageSeverity, const char*)>& func =
            *static_cast<std::function<void(EMessageSeverity, const char*)>*>(
                pUserData);

        EMessageSeverity severity = EMessageSeverity::ERROR;
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severity = EMessageSeverity::VERBOSE;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severity = EMessageSeverity::INFO;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severity = EMessageSeverity::WARNING;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severity = EMessageSeverity::ERROR;
            break;
        default:
            break;
        }

        func(severity, pCallbackData->pMessage);

        return VK_FALSE;
    }

    ContextBuilder::ContextBuilder()
    {
        _impl = new ContextBuilderImpl;
    }

    ContextBuilder::~ContextBuilder()
    {
        delete _impl;
    }

    ContextBuilder& ContextBuilder::applicationName(const std::string& name)
    {
        _impl->appName = name;
        return *this;
    }

    ContextBuilder& ContextBuilder::applicationVersion(const uint32_t major,
                                                       const uint32_t minor,
                                                       const uint32_t revision)
    {
        _impl->appVersion = VK_MAKE_VERSION(major, minor, revision);
        return *this;
    }

    ContextBuilder& ContextBuilder::engineName(const std::string& name)
    {
        _impl->engineName = name;
        return *this;
    }

    ContextBuilder& ContextBuilder::engineVersion(const uint32_t major,
                                                  const uint32_t minor,
                                                  const uint32_t revision)
    {
        _impl->engineVersion = VK_MAKE_VERSION(major, minor, revision);
        return *this;
    }

    ContextBuilder& ContextBuilder::validation(
        std::function<void(EMessageSeverity, std::string)> callback)
    {
        _impl->debugCallback = std::move(callback);
        return *this;
    }

    IContext* ContextBuilder::build() const
    {
        static bool init = gladLoaderLoadVulkan(VK_NULL_HANDLE, VK_NULL_HANDLE,
                                                VK_NULL_HANDLE) > 0;

        if (!init)
        {
            exit(-1);
        }

        {
            // get GLFW required extensions
            uint32_t extensionCount;
            const char** extensions =
                glfwGetRequiredInstanceExtensions(&extensionCount);
            for (uint32_t i = 0; i < extensionCount; i++)
            {
                const char* extension = extensions[i];
                _impl->extensions.emplace_back(extension);
            }

            if (_impl->debugCallback)
            {
                _impl->extensions.emplace_back(debug_extension);
                _impl->layers.emplace_back(validation_layer);
            }
        }

        VulkanContext* ctx = new VulkanContext;
        ctx->debugCallback = _impl->debugCallback;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.applicationVersion = _impl->appVersion;
        appInfo.engineVersion = _impl->engineVersion;
        appInfo.pApplicationName = _impl->appName.c_str();
        appInfo.pEngineName = _impl->engineName.c_str();

        vector<const char*> layers;
        vector<const char*> extensions;

        for (const auto& layer : _impl->layers)
        {
            layers.push_back(layer.c_str());
        }

        for (const auto& extension : _impl->extensions)
        {
            extensions.push_back(extension.c_str());
        }

        VkInstanceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.enabledLayerCount = static_cast<uint32_t>(_impl->layers.size());
        info.ppEnabledLayerNames = layers.data();
        info.enabledExtensionCount =
            static_cast<uint32_t>(_impl->extensions.size());
        info.ppEnabledExtensionNames = extensions.data();
        info.pApplicationInfo = &appInfo;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = &ctx->debugCallback;

        if (_impl->debugCallback)
        {
            info.pNext = &debugCreateInfo;
        }

        if (vkCreateInstance(&info, nullptr, &ctx->instance) != VK_SUCCESS)
        {
            delete ctx;
            ctx = nullptr;
        }

        if (ctx && _impl->debugCallback)
        {
            const auto func =
                reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                    vkGetInstanceProcAddr(ctx->instance,
                                          "vkCreateDebugUtilsMessengerEXT"));
            if (func != nullptr)
            {
                func(ctx->instance, &debugCreateInfo, nullptr,
                     &ctx->debugMessenger);
            }
            else
            {
                fprintf(
                    stderr,
                    "Failed to load function vkCreateDebugUtilsMessengerEXT.  "
                    "Extension not present.");
                delete ctx;
                ctx = nullptr;
            }
        }

        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, nullptr);
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, devices.data());

        for (const auto& dev : devices)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);

            VulkanPhysicalDevice* device = new VulkanPhysicalDevice;
            device->device = dev;
            device->deviceName = props.deviceName;
            device->deviceType =
                static_cast<EPhysicalDeviceType>(props.deviceType);
            device->context = ctx;

            ctx->devices.push_back(device);
        }

        return ctx;
    }

    VulkanContext::~VulkanContext()
    {
        if (!destroyed)
        {
            destroyed = true;

            for (const auto& dev : devices)
            {
                delete dev;
            }

            if (debugMessenger)
            {
                const auto func =
                    reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                        vkGetInstanceProcAddr(
                            instance, "vkDestroyDebugUtilsMessengerEXT"));
                if (func != nullptr)
                {
                    func(instance, debugMessenger, nullptr);
                }
                else
                {
                    fprintf(stderr, "Failed to load function "
                                    "vkCreateDebugUtilsMessengerEXT.  "
                                    "Extension not present.");
                }
            }
            vkDestroyInstance(instance, nullptr);
        }
    }

    vector<IPhysicalDevice*> VulkanContext::physicalDevices() const
    {
        vector<IPhysicalDevice*> devs;

        for (const auto& dev : devices)
        {
            devs.push_back(dev);
        }

        return devs;
    }
} // namespace helios