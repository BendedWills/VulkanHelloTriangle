#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct AttachmentReference
    {
        uint32_t attachment;
        VkImageLayout layout;
    };
}