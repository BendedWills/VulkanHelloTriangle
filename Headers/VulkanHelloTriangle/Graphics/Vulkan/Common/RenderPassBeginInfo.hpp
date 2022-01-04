#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/RenderPass.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Framebuffer.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct RenderPassBeginInfo
    {
        RenderPass* pRenderPass;
        Framebuffer* pFramebuffer;
        VkRect2D renderArea;
        uint32_t clearValueCount;
        const VkClearValue* pClearValues;
    };
}