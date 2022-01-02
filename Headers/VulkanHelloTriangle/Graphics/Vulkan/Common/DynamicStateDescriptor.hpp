#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct DynamicStateDescriptor
    {
        uint64_t dynamicStateCount;
        VkDynamicState* pDynamicStates;
    };
}