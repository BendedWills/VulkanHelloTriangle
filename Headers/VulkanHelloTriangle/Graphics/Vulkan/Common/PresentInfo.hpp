#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Semaphore.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Swapchain.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct PresentInfo
    {
        uint32_t waitSemaphoreCount;
        Semaphore** ppWaitSemaphores;
        uint32_t swapchainCount;
        Swapchain** ppSwapchains;
        uint32_t* pImageIndices;
        VkResult* pResults;
    };
}