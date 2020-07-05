#include <helios/render/vk/vk_pipeline_layout.hpp>

#include <helios/containers/utility.hpp>
#include <helios/containers/vector.hpp>
#include <helios/render/bldr/pipeline_layout_builder_impl.hpp>
#include <helios/render/vk/vk_descriptor_set_layout.hpp>
#include <helios/render/vk/vk_device.hpp>

#include <glad/vulkan.h>

#include <algorithm>

namespace helios
{
    PipelineLayoutBuilder::PipelineLayoutBuilder()
    {
        _impl = new PipelineLayoutBuilderImpl;
    }

    PipelineLayoutBuilder::~PipelineLayoutBuilder()
    {
        delete _impl;
    }

    PipelineLayoutBuilder& PipelineLayoutBuilder::device(const IDevice* device)
    {
        _impl->device = const_cast<IDevice*>(device);
        return *this;
    }

    PipelineLayoutBuilder& PipelineLayoutBuilder::layouts(const vector<IDescriptorSetLayout*>& layouts)
    {
        _impl->layouts = layouts;
        return *this;
    }

    PipelineLayoutBuilder& PipelineLayoutBuilder::pushConstants(const vector<PushConstantRange>& ranges)
    {
        _impl->ranges = ranges;
        return *this;
    }

    IPipelineLayout* PipelineLayoutBuilder::build() const
    {
        VulkanPipelineLayout* pipelineLayout = new VulkanPipelineLayout;
        pipelineLayout->device = _impl->layouts.empty() ? cast<VulkanDevice*>(_impl->device)
                                                        : cast<VulkanDescriptorSetLayout*>(_impl->layouts[0])->device;

        for (const auto& layout : _impl->layouts)
        {
            pipelineLayout->layouts.push_back(cast<VulkanDescriptorSetLayout*>(layout));
        }

        pipelineLayout->ranges = _impl->ranges;

        vector<VkDescriptorSetLayout> layouts;
        vector<VkPushConstantRange> ranges;

        for (const auto range : _impl->ranges)
        {
            ranges.push_back({range.stages, range.offset, range.size});
        }

        for (const auto& layout : _impl->layouts)
        {
            layouts.push_back(cast<VulkanDescriptorSetLayout*>(layout)->layout);
        }

        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pushConstantRangeCount = static_cast<u32>(ranges.size());
        info.setLayoutCount = static_cast<u32>(layouts.size());
        info.pPushConstantRanges = ranges.data();
        info.pSetLayouts = layouts.data();

        vkCreatePipelineLayout(pipelineLayout->device->device, &info, nullptr, &pipelineLayout->layout);
        pipelineLayout->device->pipelineLayouts.push_back(pipelineLayout);

        return pipelineLayout;
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        if (!destroyed)
        {
            destroyed = true;

            if (!device->destroyed)
            {
                device->pipelineLayouts.erase(
                    std::find(device->pipelineLayouts.begin(), device->pipelineLayouts.end(), this));
            }

            vkDestroyPipelineLayout(device->device, layout, nullptr);
        }
    }
} // namespace helios