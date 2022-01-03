#include <VulkanHelloTriangle/Graphics/Vulkan/Swapchain.hpp>

#include <cmath>

using namespace Vulkan;

bool Swapchain::Init(Surface* pSurface, Device* pDevice,
	uint64_t width, uint64_t height,
	SwapchainDescriptor* swapchainDesc)
{
	if (initialized)
		return false;
	
	this->pDevice = pDevice;
	this->imageFormat = swapchainDesc->imageFormat;
	this->width = width;
	this->height = height;
	
	SwapchainDetails details = swapchainDesc->details;
	VkPresentModeKHR presentMode = ChoosePresentMode(details.presentModes);
	this->extent = ChooseExtent(details.capabilities, width, height);
	
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

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *pSurface->GetSurface();

	// Image stuff
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = swapchainDesc->imageFormat;
	createInfo.imageColorSpace = swapchainDesc->imageColorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;

	// This will probably be changed later on to take in a parameter in this
	// function.
	// This parameter in the swapchain specifies what the images in the
	// swapchain are used for.
	// This may seem familiar if you have done computer graphics before,
	// as this is only the color attachment. There are others too, such as
	// the depth buffer.
	createInfo.imageUsage = swapchainDesc->imageUsage;

	switch (swapchainDesc->imageSharingMode)
	{
		case VkSharingMode::VK_SHARING_MODE_CONCURRENT:
		{
			if (!swapchainDesc->familyIndices.hasPresentFamily)
				return false;
			
			uint32_t queueFamilyIndices[] = 
			{
				swapchainDesc->familyIndices.graphicsFamilyIndex, 
				swapchainDesc->familyIndices.presentFamilyIndex
			};

			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		break;

		case VkSharingMode::VK_SHARING_MODE_EXCLUSIVE:
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		break;

		default: 
			return false;
	}

	createInfo.preTransform = swapchainDesc->preTransform;
	createInfo.compositeAlpha = swapchainDesc->compositeAlpha;
	createInfo.presentMode = presentMode;
	createInfo.clipped = true;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(*pDevice->GetDevice(), 
		&createInfo, nullptr, &swapchain))
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

std::vector<Ref<Image>> Swapchain::GetImages()
{
	uint32_t numImages;
	vkGetSwapchainImagesKHR(*pDevice->GetDevice(), swapchain, &numImages,
		nullptr);
	
	std::vector<VkImage> swapchainImages(numImages);
	vkGetSwapchainImagesKHR(*pDevice->GetDevice(), swapchain, &numImages,
		swapchainImages.data());

	std::vector<Ref<Image>> vkHelloImages;
	for (VkImage image : swapchainImages)
		vkHelloImages.push_back(RefTools::Create<Image>(&image));
	
	return vkHelloImages;
}

bool Swapchain::GetImageViews(std::vector<Ref<ImageView>>* pVec)
{
	std::vector<Ref<Image>> images = GetImages();

	pVec->resize(images.size());
	for (uint64_t i = 0; i < images.size(); i++)
	{
		Ref<Image> image = images[i];

		ImageViewDescriptor desc;
		desc.format = imageFormat;
		desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
		desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		desc.subresourceRange.baseMipLevel = 0;
		desc.subresourceRange.levelCount = 1;
		desc.subresourceRange.baseArrayLayer = 0;
		desc.subresourceRange.layerCount = 1;

		pVec->at(i) = RefTools::Create<ImageView>();
		if (!pVec->at(i)->Init(pDevice, image.get(), &desc))
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

VkSwapchainKHR* Swapchain::GetSwapchain()
{
	return &swapchain;
}

void Swapchain::OnDispose()
{
	vkDestroySwapchainKHR(*pDevice->GetDevice(), swapchain, nullptr);
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