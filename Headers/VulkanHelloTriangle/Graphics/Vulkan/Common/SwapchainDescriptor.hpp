#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/SwapchainDetails.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/QueueFamilyIndices.hpp>

namespace Vulkan
{
    struct SwapchainDescriptor
    {
        VkFormat imageFormat;
        VkColorSpaceKHR imageColorSpace;
        VkImageUsageFlags imageUsage;
        VkSharingMode imageSharingMode;
        VkSurfaceTransformFlagBitsKHR preTransform;
        VkCompositeAlphaFlagBitsKHR compositeAlpha;
        QueueFamilyIndices familyIndices;
        SwapchainDetails details;
    };
}