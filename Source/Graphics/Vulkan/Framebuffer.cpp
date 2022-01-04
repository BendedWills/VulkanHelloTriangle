#include <VulkanHelloTriangle/Graphics/Vulkan/Framebuffer.hpp>

#include <stdio.h>
#include <stdlib.h>

using namespace Vulkan;

bool Framebuffer::Init(Device* pDevice, FramebufferDescriptor* pDesc)
{
    if (initialized
        || !pDesc->pRenderPass
        ||  pDesc->attachmentCount == 0
        || !pDesc->ppAttachments)
        return false;
    
    this->pDevice = pDevice;

    VkImageView* attachments = (VkImageView*)malloc(sizeof(VkImageView)
        * pDesc->attachmentCount);
    if (!attachments)
        return false;
    
    for (uint64_t i = 0; i < pDesc->attachmentCount; i++)
        attachments[i] = *pDesc->ppAttachments[i]->GetImageView();

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = *pDesc->pRenderPass->GetRenderPass();
    createInfo.attachmentCount = pDesc->attachmentCount;
    createInfo.pAttachments = attachments;
    createInfo.width = pDesc->width;
    createInfo.height = pDesc->height;
    createInfo.layers = pDesc->layers;

    if (vkCreateFramebuffer(*pDevice->GetDevice(), &createInfo, nullptr,
        &framebuffer) != VK_SUCCESS)
    {
        free(attachments);
        return false;
    }

    free(attachments);

    initialized = true;
    return true;
}

VkFramebuffer* Framebuffer::GetFramebuffer()
{
    return &framebuffer;
}

void Framebuffer::OnDispose()
{
    vkDestroyFramebuffer(*pDevice->GetDevice(), framebuffer, nullptr);
}