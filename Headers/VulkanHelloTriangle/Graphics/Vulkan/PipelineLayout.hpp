#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/PipelineLayoutDescriptor.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class PipelineLayout : public IDisposable
    {
    public:
        PipelineLayout() {}

		PipelineLayout(const PipelineLayout&) = delete;
		PipelineLayout(PipelineLayout&&) = delete;
		PipelineLayout& operator=(const PipelineLayout&) = delete;
		PipelineLayout& operator=(PipelineLayout&&) = delete;

        bool Init(Device* pDevice, PipelineLayoutDescriptor* pDesc);
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;
        VkPipelineLayout pipelineLayout;
    };
}