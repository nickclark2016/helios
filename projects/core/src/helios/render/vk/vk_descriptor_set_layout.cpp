#include <helios/render/vk/vk_descriptor_set_layout.hpp>

#include <helios/render/bldr/descriptor_set_layout_builder_impl.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder()
    {
        _impl = new DescriptorSetLayoutBuilderImpl;
    }

    DescriptorSetLayoutBuilder::~DescriptorSetLayoutBuilder()
    {
        delete _impl;
    }

    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::device(
        const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::bindings(
        const vector<DescriptorSetLayoutBinding>& bindings)
    {
        _impl->bindings = bindings;
        return *this;
    }

    IDescriptorSetLayout* DescriptorSetLayoutBuilder::build() const
    {
        VulkanDescriptorSetLayout* layout = new VulkanDescriptorSetLayout;
        layout->device = cast<VulkanDevice*>(_impl->device);
        layout->layoutBindings = _impl->bindings;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        vector<VkDescriptorSetLayoutBinding> bindings;

        for (const auto& binding : _impl->bindings)
        {
            bindings.push_back({binding.binding,
                                static_cast<VkDescriptorType>(binding.type),
                                binding.count, binding.stages, nullptr});
        }

        info.bindingCount = static_cast<u32>(bindings.size());
        info.pBindings = bindings.data();

        vkCreateDescriptorSetLayout(layout->device->device, &info, nullptr,
                                    &layout->layout);

		layout->device->setLayouts.push_back(layout);

        return layout;
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->setLayouts.erase(std::find(device->setLayouts.begin(),
                                                   device->setLayouts.end(),
                                                   this));
            }

            vkDestroyDescriptorSetLayout(device->device, layout, nullptr);
        }
    }

    vector<DescriptorSetLayoutBinding> VulkanDescriptorSetLayout::bindings()
        const
    {
        return layoutBindings;
    }
} // namespace helios