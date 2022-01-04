#include <VulkanHelloTriangle/Graphics/Vulkan/Queue.hpp>

#include <stdio.h>
#include <stdlib.h>

using namespace Vulkan;

Queue::Queue(const Queue& ref)
{
    this->queue = ref.queue;
}

VkQueue* Queue::GetQueue()
{
    return &queue;
}

bool Queue::Submit(uint32_t submitCount, VkSubmitInfo* pSubmits)
{
    return vkQueueSubmit(queue, submitCount, pSubmits, VK_NULL_HANDLE) 
        == VK_SUCCESS;
}

void Queue::Present(VkPresentInfoKHR* pInfo)
{
    vkQueuePresentKHR(queue, pInfo);
}

Queue::Queue(VkQueue* pQueue)
{
    this->queue = *pQueue;
}
