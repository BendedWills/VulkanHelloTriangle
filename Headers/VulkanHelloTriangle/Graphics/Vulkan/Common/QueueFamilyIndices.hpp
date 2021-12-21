#pragma once

#include <iostream>

namespace Vulkan
{
    struct QueueFamilyIndices
	{
		bool hasGraphicsFamily = false;
		bool hasPresentFamily = false;
		uint32_t graphicsFamilyIndex = 0;
		uint32_t presentFamilyIndex = 0;
	};
}