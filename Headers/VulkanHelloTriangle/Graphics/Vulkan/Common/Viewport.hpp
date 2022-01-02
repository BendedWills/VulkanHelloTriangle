#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/Rect2D.hpp>
#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct Viewport : public Rect2D
    {
        float minDepth = 0.0f;
        float maxDepth = 1.0f;
    };
}