#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/RenderPassDescriptor.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class RenderPass : public IDisposable
    {
    public:
        RenderPass() {}

		RenderPass(const RenderPass&) = delete;
		RenderPass(RenderPass&&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;
		RenderPass& operator=(RenderPass&&) = delete;

        bool Init(Device* pDevice, RenderPassDescriptor* pDesc);

        VkRenderPass* GetRenderPass();
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;
        VkRenderPass renderPass;
    };
}