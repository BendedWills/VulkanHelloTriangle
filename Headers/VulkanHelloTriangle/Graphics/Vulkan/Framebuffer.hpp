#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/FramebufferDescriptor.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Framebuffer : public IDisposable
    {
    public:
        Framebuffer() {}

		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = delete;

        /**
         * @brief Initializes the image view.
         * 
         * @param pDevice A pointer to the device this image view is being
         *  created for.
         * @param pDesc The framebuffer descriptor.
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(Device* pDevice, FramebufferDescriptor* pDesc);
        
        VkFramebuffer* GetFramebuffer();
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;

        VkFramebuffer framebuffer;
    };
}