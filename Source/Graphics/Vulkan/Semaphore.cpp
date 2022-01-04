#include <VulkanHelloTriangle/Graphics/Vulkan/Semaphore.hpp>

using namespace Vulkan;

bool Semaphore::Init(Device* pDevice)
{
    if (initialized)
        return false;

    this->pDevice = pDevice;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(*pDevice->GetDevice(), &createInfo, nullptr,
        &semaphore) != VK_SUCCESS)
        return false;

    initialized = true;
    return true;
}

uint32_t Semaphore::AcquireNextImage(Swapchain* pSwapchain, uint64_t timeout)
{
    uint32_t index;
    vkAcquireNextImageKHR(*pDevice->GetDevice(), *pSwapchain->GetSwapchain(),
        timeout, semaphore, VK_NULL_HANDLE, &index);
    
    return index;
}

VkSemaphore* Semaphore::GetSemaphore()
{
    return &semaphore;
}

void Semaphore::OnDispose()
{
    vkDestroySemaphore(*pDevice->GetDevice(), semaphore, nullptr);
}