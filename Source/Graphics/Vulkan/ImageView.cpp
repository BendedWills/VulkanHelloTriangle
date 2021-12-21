#include <VulkanHelloTriangle/Graphics/Vulkan/ImageView.hpp>

using namespace Vulkan;

bool ImageView::Init(Device* pDevice)
{
    
}

void ImageView::OnDispose()
{
    vkDestroyImageView(*pDevice->GetDevice(), imageView, nullptr);
}
