#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct DepthStencilDescriptor
    {
        VkPipelineDepthStencilStateCreateFlags flags;
        VkBool32 depthTestEnable;
        VkBool32 depthWriteEnable;
        VkCompareOp depthCompareOp;
        VkBool32 depthBoundsTestEnable;
        VkBool32 stencilTestEnable;
        VkStencilOpState front;
        VkStencilOpState back;
        float minDepthBounds;
        float maxDepthBounds;
    };
}