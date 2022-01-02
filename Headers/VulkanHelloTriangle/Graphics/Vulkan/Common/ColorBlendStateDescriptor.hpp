#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct ColorBlendStateDescriptor
    {
        VkBool32 logicOpEnable;
        VkLogicOp logicOp;
        uint32_t attachmentCount;
        const VkPipelineColorBlendAttachmentState* pAttachments;
        float blendConstants[4];
    };
}