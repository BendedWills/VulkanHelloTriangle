/*
 * This header defines a Vulkan queue. Vulkan needs queues for any operation.
 */

#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/SubmitInfo.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/PresentInfo.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Queue
    {
        friend Device;
    public:
        Queue() {}
        Queue(const Queue& ref);

        bool Submit(uint32_t submitCount, VkSubmitInfo* pSubmits);
        void Present(VkPresentInfoKHR* pInfo);
        
        VkQueue* GetQueue();
    protected:
        Queue(VkQueue* pQueue);
    private:
        VkQueue queue;
    };
}