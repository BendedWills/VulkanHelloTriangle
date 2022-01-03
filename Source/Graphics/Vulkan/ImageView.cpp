#include <VulkanHelloTriangle/Graphics/Vulkan/ImageView.hpp>

using namespace Vulkan;

bool ImageView::Init(Device* pDevice, Image* pImage, 
    ImageViewDescriptor* pDesc)
{
    return Init(pDevice, *pImage->GetImage(), pDesc);
}

bool ImageView::Init(Device* pDevice, VkImage image, 
    ImageViewDescriptor* pDesc)
{
    if (initialized)
        return false;
    
    this->pDevice = pDevice;
    
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;

    createInfo.viewType = pDesc->viewType;
    createInfo.format = pDesc->format;
    createInfo.components.r = pDesc->components.r;
    createInfo.components.g = pDesc->components.g;
    createInfo.components.b = pDesc->components.b;
    createInfo.components.a = pDesc->components.a;
    createInfo.subresourceRange = pDesc->subresourceRange;

    if (vkCreateImageView(*pDevice->GetDevice(), &createInfo, nullptr,
        &imageView))
        return false;

    initialized = true;
    return true;
}

VkImageView* ImageView::GetImageView()
{
    return &imageView;
}

void ImageView::OnDispose()
{
    vkDestroyImageView(*pDevice->GetDevice(), imageView, nullptr);
}
