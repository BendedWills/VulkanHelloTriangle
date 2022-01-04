#include <VulkanHelloTriangle/Graphics/Vulkan/CommandBuffer.hpp>

using namespace Vulkan;

CommandBuffer::CommandBuffer(VkCommandBuffer* commandBuffer)
{
    this->commandBuffer = *commandBuffer;
}

CommandBuffer::CommandBuffer(const CommandBuffer& ref)
{
    commandBuffer = ref.commandBuffer;
}

bool CommandBuffer::Begin(
    VkCommandBufferUsageFlags flags,
    VkCommandBufferInheritanceInfo* pInheritanceInfo
)
{
    VkCommandBufferBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = flags;
    info.pInheritanceInfo = pInheritanceInfo;

    return vkBeginCommandBuffer(commandBuffer, &info) == VK_SUCCESS;
}

void CommandBuffer::BeginRenderPass(RenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents)
{
    VkRenderPassBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = *pRenderPassBegin->pRenderPass->GetRenderPass();
    info.framebuffer = *pRenderPassBegin->pFramebuffer->GetFramebuffer();
    info.renderArea = pRenderPassBegin->renderArea;
    info.clearValueCount = pRenderPassBegin->clearValueCount;
    info.pClearValues = pRenderPassBegin->pClearValues;

    vkCmdBeginRenderPass(commandBuffer, &info, contents);
}

void CommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, 
    Pipeline* pPipeline)
{
    BindPipeline(bindPoint, *pPipeline->GetPipeline());
}

void CommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, 
    VkPipeline pipeline)
{
    vkCmdBindPipeline(commandBuffer, bindPoint, pipeline);
}

void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, 
    uint32_t startVertex, uint32_t firstInstance)
{
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, startVertex, 
        firstInstance);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

bool CommandBuffer::End()
{
    return vkEndCommandBuffer(commandBuffer) == VK_SUCCESS;
}

bool CommandBuffer::Reset()
{
    return vkResetCommandBuffer(commandBuffer, 0) == VK_SUCCESS;
}

VkCommandBuffer* CommandBuffer::GetCommandBuffer()
{
    return &commandBuffer;
}