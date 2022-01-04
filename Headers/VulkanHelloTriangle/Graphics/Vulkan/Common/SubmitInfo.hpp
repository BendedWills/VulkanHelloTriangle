#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Semaphore.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/CommandBuffer.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct SubmitInfo
    {
        uint32_t waitSemaphoreCount;
        Semaphore** ppWaitSemaphores;
        VkPipelineStageFlags* pWaitDstStageMask;
        uint32_t commandBufferCount;
        CommandBuffer** ppCommandBuffers;
        uint32_t signalSemaphoreCount;
        Semaphore** ppSignalSemaphores;
    };
}