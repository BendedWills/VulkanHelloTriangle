#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/RenderPass.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/ImageView.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct FramebufferDescriptor
    {
        RenderPass* pRenderPass;
        uint32_t attachmentCount;
        ImageView** ppAttachments;
        uint64_t width;
        uint64_t height;
        uint32_t layers;
    };
}