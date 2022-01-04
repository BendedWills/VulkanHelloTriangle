#include <VulkanHelloTriangle/Graphics/Vulkan/CommandPool.hpp>

#include <stdio.h>
#include <stdlib.h>

using namespace Vulkan;

bool CommandPool::Init(Device* pDevice, uint32_t queueFamilyIndex, 
    VkCommandPoolCreateFlags flags)
{
    if (initialized)
        return false;
    
    this->pDevice = pDevice;
    
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    createInfo.flags = flags;

    if (vkCreateCommandPool(*pDevice->GetDevice(), &createInfo, nullptr,
        &commandPool))
        return false;

    initialized = true;
    return true;
}

bool CommandPool::AllocateCommandBuffers(
    VkCommandBufferLevel level,
    uint32_t commandBufferCount,
    CommandBuffer* pBuffers
)
{
    if (!pBuffers || !initialized)
        return false;
    
    VkCommandBuffer* buffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer)
        * commandBufferCount);
    if (!buffers)
        return false;
    
    memset(buffers, 0, sizeof(VkCommandBuffer) * commandBufferCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = commandBufferCount;

    if (vkAllocateCommandBuffers(*pDevice->GetDevice(), &allocInfo, buffers)
        != VK_SUCCESS)
    {
        free(buffers);
        return false;
    }

    for (uint32_t i = 0; i < commandBufferCount; i++)
        *pBuffers[i].GetCommandBuffer() = buffers[i];

    free(buffers);
    return true;
}

bool CommandPool::Reset()
{
    return vkResetCommandPool(*pDevice->GetDevice(), commandPool, 0) == VK_SUCCESS;
}

void CommandPool::OnDispose()
{
    vkDestroyCommandPool(*pDevice->GetDevice(), commandPool, nullptr);
}