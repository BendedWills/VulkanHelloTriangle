/*
 * This header defines a Vulkan queue. Vulkan needs queues for any operation.
 */

#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Device;
    class Queue
    {
        friend Device;
    public:
        Queue() {}
        Queue(const Queue& ref);
        
        VkQueue* GetQueue();
    protected:
        Queue(VkQueue* pQueue);
    private:
        VkQueue queue;
    };
}