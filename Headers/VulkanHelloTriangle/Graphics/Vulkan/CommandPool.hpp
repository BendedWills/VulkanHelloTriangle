#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/CommandBuffer.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class CommandPool : public IDisposable
    {
    public:
        CommandPool() {}

		CommandPool(const CommandPool&) = delete;
		CommandPool(CommandPool&&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;
		CommandPool& operator=(CommandPool&&) = delete;

        /**
         * @brief Initializes the image view.
         * 
         * @param pDevice A pointer to the device this command pool is being
         *  created for.
         * @param queueFamilyIndex The index of the queue to create the pool for.
         * @param flags Flags.
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(Device* pDevice, uint32_t queueFamilyIndex, 
            VkCommandPoolCreateFlags flags);

        bool AllocateCommandBuffers(
            VkCommandBufferLevel level,
            uint32_t commandBufferCount,
            CommandBuffer* pBuffers
        );

        bool Reset();
        
        VkCommandPool* GetCommandPool();
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;

        VkCommandPool commandPool;
    };
}