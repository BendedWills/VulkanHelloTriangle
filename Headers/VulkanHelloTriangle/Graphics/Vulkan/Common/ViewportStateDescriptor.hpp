#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/Rect2D.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/Viewport.hpp>
#include <iostream>

namespace Vulkan
{
    struct ViewportStateDescriptor
    {
        uint64_t viewportCount;
        Viewport* pViewports;
        uint64_t scissorCount;
        Rect2D* pScissors;
    };
}