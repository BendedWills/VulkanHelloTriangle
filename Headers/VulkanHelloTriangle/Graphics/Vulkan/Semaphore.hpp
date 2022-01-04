#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Swapchain.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Semaphore : public IDisposable
    {
    public:
        Semaphore() {}

		Semaphore(const Semaphore&) = delete;
		Semaphore(Semaphore&&) = delete;
		Semaphore& operator=(const Semaphore&) = delete;
		Semaphore& operator=(Semaphore&&) = delete;

        /**
         * @brief Initializes the semaphore.
         * 
         * @param pDevice A pointer to the device this semaphore is being
         *  created for.
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(Device* pDevice);
        
        uint32_t AcquireNextImage(Swapchain* pSwapchain, uint64_t timeout);

        VkSemaphore* GetSemaphore();
    protected:
        void OnDispose();
    private:
        Device* pDevice;
        VkSemaphore semaphore;
    };
}