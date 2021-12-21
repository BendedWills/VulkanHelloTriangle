/*
 * This file defines a structure that contains the details about a device's
 * support of a swapchain.
 */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan
{
    struct SwapchainDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
}