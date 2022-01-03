#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/AttachmentReference.hpp>
#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct SubpassDescriptor
    {
        VkPipelineBindPoint pipelineBindPoint;
        uint32_t colorAttachmentCount;
        AttachmentReference* pColorAttachments;
    };
}