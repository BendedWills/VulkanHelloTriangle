#include <VulkanHelloTriangle/Graphics/Vulkan/PipelineLayout.hpp>

using namespace Vulkan;

bool PipelineLayout::Init(Device* pDevice, PipelineLayoutDescriptor* pDesc)
{
    if (initialized)
		return false;
    
    this->pDevice = pDevice;
    
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = pDesc->setLayoutCount;
    createInfo.pSetLayouts = pDesc->pSetLayouts;
    createInfo.pushConstantRangeCount = pDesc->pushConstantRangeCount;
    createInfo.pPushConstantRanges = pDesc->pPushConstantRanges;

    if (vkCreatePipelineLayout(*pDevice->GetDevice(), &createInfo, nullptr,
        &pipelineLayout) != VK_SUCCESS)
        return false;
    
    initialized = true;
    return true;
}

VkPipelineLayout* PipelineLayout::GetPipelineLayout()
{
    return &pipelineLayout;
}

void PipelineLayout::OnDispose()
{
    vkDestroyPipelineLayout(*pDevice->GetDevice(), pipelineLayout, nullptr);
}