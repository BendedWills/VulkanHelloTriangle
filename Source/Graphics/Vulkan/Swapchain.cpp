#include <VulkanHelloTriangle/Graphics/Vulkan/Swapchain.hpp>

#include <cmath>

using namespace Vulkan;

bool Swapchain::Init(
	Surface* pSurface,
	Device* pDevice,
	uint64_t width, uint64_t height,
	SwapchainDetails details,
	VkSwapchainCreateInfoKHR* createInfo
)
{
	if (initialized)
		return false;
	
	this->pDevice = pDevice;
	this->imageFormat = createInfo->imageFormat;
	this->width = width;
	this->height = height;
	this->extent = createInfo->imageExtent;
	
	// I don't know how else to explain this so I'm just going to quote this
	// from vulkan-tutorial.com:
	// "
	// However, simply sticking to this minimum means that we may 
	// sometimes have to wait on the driver to complete internal operations 
	// before we can acquire another image to render to. 
	// Therefore it is recommended to request at least one more image than 
	// the minimum:
	// "
	imageCount = details.capabilities.minImageCount + 1;

	// Adding to the image count may have it go over the maximum
	// so here I'm checking if it does, and if so, setting the image count to
	// its maximum value.
	uint32_t max = details.capabilities.maxImageCount;
	if (max > 0 && imageCount > max)
		imageCount = max;

	if (vkCreateSwapchainKHR(pDevice->Get(), 
		createInfo, nullptr, &swapchain))
		return false;
	
	initialized = true;
	return true;
}

VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormat(SwapchainDetails details) 
{
    for (const auto& availableFormat : details.formats)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return details.formats[0];
}

uint32_t Swapchain::GetImageCount()
{
	return imageCount;
}

std::vector<VkImage> Swapchain::GetImages()
{
	uint32_t numImages;
	vkGetSwapchainImagesKHR(pDevice->Get(), swapchain, &numImages,
		nullptr);
	
	std::vector<VkImage> swapchainImages(numImages);
	vkGetSwapchainImagesKHR(pDevice->Get(), swapchain, &numImages,
		swapchainImages.data());

	return swapchainImages;
}

bool Swapchain::GetImageViews(std::vector<VkImageView>* pVec)
{
	std::vector<VkImage> images = GetImages();

	pVec->resize(images.size());
	for (uint64_t i = 0; i < images.size(); i++)
	{
		VkImage image = images[i];

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		
		if (vkCreateImageView(pDevice->Get(), &createInfo, nullptr,
			&pVec->at(i)) != VK_SUCCESS)
			return false;
	}

	return true;
}

VkExtent2D Swapchain::GetExtent()
{
	return extent;
}

VkFormat Swapchain::GetImageFormat()
{
	return imageFormat;
}

uint64_t Swapchain::GetWidth()
{
	return width;
}

uint64_t Swapchain::GetHeight()
{
	return height;
}

VkSwapchainKHR Swapchain::Get()
{
	return swapchain;
}

void Swapchain::OnDispose()
{
	vkDestroySwapchainKHR(pDevice->Get(), swapchain, nullptr);
}

VkPresentModeKHR Swapchain::ChoosePresentMode(
	std::vector<VkPresentModeKHR>& availablePresentModes) 
{
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::ChooseExtent(
	VkSurfaceCapabilitiesKHR& capabilities,
	uint64_t width, uint64_t height) 
{
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;
		
	VkExtent2D extent = { (uint32_t)width, (uint32_t)height };

	extent.width = std::clamp(extent.width, 
		capabilities.minImageExtent.width, 
		capabilities.maxImageExtent.width
	);

	extent.height = std::clamp(extent.height, 
		capabilities.minImageExtent.height, 
		capabilities.maxImageExtent.height
	);

	return extent;
}