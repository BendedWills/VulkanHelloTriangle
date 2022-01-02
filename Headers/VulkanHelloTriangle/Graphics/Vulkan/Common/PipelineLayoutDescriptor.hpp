#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct PipelineLayoutDescriptor
    {
        uint32_t setLayoutCount;
        const VkDescriptorSetLayout* pSetLayouts;
        uint32_t pushConstantRangeCount;
        const VkPushConstantRange* pPushConstantRanges;
    };
}