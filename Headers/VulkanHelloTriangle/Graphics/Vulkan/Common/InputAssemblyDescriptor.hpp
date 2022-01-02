#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct InputAssemblyDescriptor
    {
        VkPrimitiveTopology topology;
        bool primitiveRestartEnable = false;
    };
}