#include <VulkanHelloTriangle/Graphics/Vulkan/Queue.hpp>

using namespace Vulkan;

Queue::Queue(const Queue& ref)
{
    this->queue = ref.queue;
}

VkQueue* Queue::GetQueue()
{
    return &queue;
}

Queue::Queue(VkQueue* pQueue)
{
    this->queue = *pQueue;
}
