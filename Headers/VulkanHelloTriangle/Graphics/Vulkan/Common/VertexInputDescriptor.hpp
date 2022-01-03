#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct VertexInputDescriptor
    {
        uint32_t                                    vertexBindingDescriptionCount;
        const VkVertexInputBindingDescription*      pVertexBindingDescriptions;
        uint32_t                                    vertexAttributeDescriptionCount;
        const VkVertexInputAttributeDescription*    pVertexAttributeDescriptions;
    };
}