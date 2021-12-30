#include <VulkanHelloTriangle/Graphics/Vulkan/Image.hpp>

using namespace Vulkan;

VkImage* Image::GetImage()
{
    return &image;
}

Image::Image(VkImage* pImage)
{
    image = *pImage;
    fromExisting = true;
}

void Image::OnDispose()
{
    if (fromExisting)
        return;
    
    // TODO: Image cleanup
}