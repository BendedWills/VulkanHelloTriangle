#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Pipeline.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/RenderPassBeginInfo.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class CommandBuffer
    {
    public:
        CommandBuffer() {}
        CommandBuffer(VkCommandBuffer* commandBuffer);
        CommandBuffer(const CommandBuffer& ref);

        bool Begin(
            VkCommandBufferUsageFlags flags,
            VkCommandBufferInheritanceInfo* pInheritanceInfo
        );

        void BeginRenderPass(RenderPassBeginInfo* pRenderPassBegin,
            VkSubpassContents contents);
        void BindPipeline(VkPipelineBindPoint bindPoint, Pipeline* pPipeline);
        void BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline);
        void Draw(uint32_t vertexCount, uint32_t instanceCount, 
            uint32_t startVertex, uint32_t firstInstance);
        void EndRenderPass();

        bool End();

        bool Reset();
        
        VkCommandBuffer* GetCommandBuffer();
    private:
        VkCommandBuffer commandBuffer;
    };
}