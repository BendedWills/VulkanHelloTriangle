#include <VulkanHelloTriangle/Graphics/Vulkan/RenderPass.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Vulkan;

bool RenderPass::Init(Device* pDevice, RenderPassDescriptor* pDesc)
{
    if (initialized)
		return false;
    
    this->pDevice = pDevice;

    if (pDesc->attachmentCount == 0 || pDesc->subpassCount == 0)
        return false;

    VkAttachmentDescription* attachments = (VkAttachmentDescription*)malloc(
        sizeof(VkAttachmentDescription) * pDesc->attachmentCount);
    if (!attachments)
        return false;
    
    memset(attachments, 0, sizeof(VkAttachmentDescription) 
        * pDesc->attachmentCount);
    
    for (uint64_t i = 0; i < pDesc->attachmentCount; i++)
    {
        AttachmentDescriptor* descriptor = pDesc->pAttachments + i;
        VkAttachmentDescription* desc = attachments + i;

        desc->format = descriptor->format;
        desc->samples = descriptor->samples;
        desc->loadOp = descriptor->loadOp;
        desc->storeOp = descriptor->storeOp;
        desc->stencilLoadOp = descriptor->stencilLoadOp;
        desc->stencilStoreOp = descriptor->stencilStoreOp;
        desc->initialLayout = descriptor->initialLayout;
        desc->finalLayout = descriptor->finalLayout;
    }

    VkSubpassDescription* subpasses = (VkSubpassDescription*)malloc(
        sizeof(VkSubpassDescription) * pDesc->subpassCount);
    if (!subpasses)
    {
        free(attachments);
        return false;
    }

    memset(subpasses, 0, sizeof(VkSubpassDescription) 
        * pDesc->subpassCount);

    for (uint64_t i = 0; i < pDesc->subpassCount; i++)
    {
        SubpassDescriptor* descriptor = pDesc->pSubpasses + i;
        VkSubpassDescription* desc = subpasses + i;

        desc->pipelineBindPoint = descriptor->pipelineBindPoint;
        desc->colorAttachmentCount = descriptor->colorAttachmentCount;
        
        VkAttachmentReference* colorAttachments = (VkAttachmentReference*)
            malloc(sizeof(VkAttachmentReference) * desc->colorAttachmentCount);
        if (!colorAttachments)
        {
            free(attachments);
            free(subpasses);

            // Free all other created subpasses
            for (uint64_t i2 = 0; i2 < i + 1; i2++)
                free((void*)subpasses[i2].pColorAttachments);

            return false;
        }

        for (uint64_t i2 = 0; i2 < desc->colorAttachmentCount; i2++)
        {
            AttachmentReference* attachment = descriptor->pColorAttachments;
            VkAttachmentReference* vkattachment = colorAttachments + i2;

            vkattachment->attachment = attachment->attachment;
            vkattachment->layout = attachment->layout;
        }

        desc->pColorAttachments = colorAttachments;
    }

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = pDesc->attachmentCount;
    createInfo.pAttachments = attachments;
    createInfo.subpassCount = pDesc->subpassCount;
    createInfo.pSubpasses = subpasses;
    
    if (vkCreateRenderPass(*pDevice->GetDevice(), &createInfo, nullptr, 
        &renderPass) != VK_SUCCESS)
    {
        free(attachments);
        free(subpasses);
        return false;
    }

    free(attachments);

    for (uint64_t i = 0; i < pDesc->subpassCount; i++)
        free((void*)subpasses[i].pColorAttachments);

    free(subpasses);
    
    initialized = true;
    return true;
}

VkRenderPass* RenderPass::GetRenderPass()
{
    return &renderPass;
}

void RenderPass::OnDispose()
{
    vkDestroyRenderPass(*pDevice->GetDevice(), renderPass, nullptr);
}