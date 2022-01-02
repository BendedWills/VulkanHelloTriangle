#include <VulkanHelloTriangle/Graphics/Vulkan/Pipeline.hpp>

using namespace Vulkan;

bool Pipeline::Init(Device* pDevice)
{
    this->pDevice = pDevice;
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    if (vkCreateGraphicsPipelines(*pDevice->GetDevice(), VK_NULL_HANDLE,
        1, &createInfo, nullptr, &pipeline))
        return false;
    
    initialized = true;
    return true;
}

void Pipeline::OnDispose()
{
    vkDestroyPipeline(*pDevice->GetDevice(), pipeline, nullptr);
}