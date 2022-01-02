#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Pipeline : public IDisposable
    {
    public:
        Pipeline() {}

		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline& operator=(Pipeline&&) = delete;

        bool Init(Device* pDevice);
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;
        VkPipeline pipeline;
    };
}