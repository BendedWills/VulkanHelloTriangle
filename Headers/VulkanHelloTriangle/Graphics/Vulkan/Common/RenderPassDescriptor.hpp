#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/AttatchmentDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/SubpassDescriptor.hpp>
#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct RenderPassDescriptor
    {
        uint32_t attachmentCount;
        AttachmentDescriptor* pAttachments;
        uint32_t subpassCount;
        SubpassDescriptor* pSubpasses;
    };
}