#include <helios/render/vk/vk_shader_module.hpp>

#include <helios/render/bldr/shader_module_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    ShaderModuleBuilder::ShaderModuleBuilder()
    {
        _impl = new ShaderModuleBuilderImpl;
    }

    ShaderModuleBuilder::~ShaderModuleBuilder()
    {
        delete _impl;
    }

    ShaderModuleBuilder& ShaderModuleBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    ShaderModuleBuilder& ShaderModuleBuilder::source(const vector<uint8_t>& source)
    {
        _impl->data = source;
        return *this;
    }

    IShaderModule* ShaderModuleBuilder::build() const
    {
        VulkanShaderModule* shader = new VulkanShaderModule;
        shader->device = cast<VulkanDevice*>(_impl->device);

        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = static_cast<u32>(_impl->data.size());
        info.pCode = reinterpret_cast<u32*>(_impl->data.data());
        vkCreateShaderModule(shader->device->device, &info, nullptr, &shader->shaderModule);
        shader->device->modules.push_back(shader);

        return shader;
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->modules.erase(std::find(device->modules.begin(), device->modules.end(), this));
            }

            vkDestroyShaderModule(device->device, shaderModule, nullptr);
        }
    }
} // namespace helios