#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct ComponentMapping
    {
        VkComponentSwizzle    r = VK_COMPONENT_SWIZZLE_R;
        VkComponentSwizzle    g = VK_COMPONENT_SWIZZLE_G;
        VkComponentSwizzle    b = VK_COMPONENT_SWIZZLE_B;
        VkComponentSwizzle    a = VK_COMPONENT_SWIZZLE_A;
    };

    struct ImageViewDescriptor
    {
        VkImageViewType         viewType;
        VkFormat                format;
        ComponentMapping        components;
        VkImageSubresourceRange subresourceRange;
    };
}