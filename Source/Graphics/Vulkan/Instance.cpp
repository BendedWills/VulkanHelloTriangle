#include <VulkanHelloTriangle/Graphics/Vulkan/Instance.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <set>

using namespace Vulkan;

static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	Instance* instance = (Instance*)pUserData;
	instance->DispatchDebugMessage(messageSeverity, messageType, 
		pCallbackData);
	
	return false;
}

static VkResult Vulkan_CreateDebugUtilsMessengerEXT(VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void Vulkan_DestroyDebugUtilsMessengerEXT(VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator) 
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT");
	
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

bool Instance::Init(
	const char* applicationName,
	const char* engineName,
	bool debugMode
)
{
	if (initialized)
		return false;
	
	this->debugMode = debugMode;

	// Most structures require an sType that specifies what this structure
	// represents.
	// This struct is needed to tell Vulkan some information about this
	// application.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	if (applicationName)
		appInfo.pApplicationName = applicationName;
	else
		appInfo.pApplicationName = "VulkanApp";
	
	if (engineName)
		appInfo.pEngineName = engineName;
	else
		appInfo.pEngineName = "yo mama so fat";

	// Info about the instance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> requiredExtentions = 
		GetRequiredExtentions(debugMode);

	createInfo.enabledExtensionCount = requiredExtentions.size();
	createInfo.ppEnabledExtensionNames = requiredExtentions.data();

	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
	if (debugMode)
	{
		// Initialize validation layers

		if (!CheckValidationLayerSupport(validationLayers))
			return false;
		
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
		
		// Initialize debug messenger
		{
			messengerCreateInfo.sType = 
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			messengerCreateInfo.messageSeverity = 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			messengerCreateInfo.messageType = 
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			messengerCreateInfo.pfnUserCallback = Vulkan_DebugCallback;
			messengerCreateInfo.pUserData = this;

			createInfo.pNext = &messengerCreateInfo;
		}
	}
	else
	{
		// If not debug mode

		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		std::cout << "Failed to create vulkan instance!" << std::endl;
		return false;
	}

	uint32_t extentionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extentionCount, nullptr);

	extentions = std::vector<VkExtensionProperties>(extentionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extentionCount,
		extentions.data());
	
	if (debugMode)
		Vulkan_CreateDebugUtilsMessengerEXT(instance, &messengerCreateInfo,
			nullptr, &debugMessenger);
	
	initialized = true;
	return true;
}

bool Instance::PickDevice(VkPhysicalDevice* pDevice, Surface* pSurface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		return false;
	
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (VkPhysicalDevice device : devices)
		if (IsDeviceSuitable(device, pSurface))
		{
			*pDevice = device;
			return true;
		}
	
	return false;
}

Vulkan::QueueFamilyIndices Instance::FindQueueFamilies(
	VkPhysicalDevice device, Surface* pSurface)
{
	QueueFamilyIndices queueFamilies;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

	if (familyCount == 0)
		return queueFamilies;

	std::vector<VkQueueFamilyProperties> families(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, 
		families.data());
	
	int i = 0;
	for (const auto& queueFamily : families)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queueFamilies.hasGraphicsFamily = true;
			queueFamilies.graphicsFamilyIndex = i;
		}

		if (pSurface)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, 
				*pSurface->GetSurface(), &presentSupport);
			if (presentSupport)
			{
				queueFamilies.hasPresentFamily = true;
				queueFamilies.presentFamilyIndex = i;
			}
		}
		
		i++;
	}

	return queueFamilies;
}

SwapchainDetails Instance::QuerySwapchainSupport(
	VkPhysicalDevice device, Surface* pSurface)
{
	VkSurfaceKHR surface = *pSurface->GetSurface();

	SwapchainDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, 
		&details.capabilities);
	
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, 
		nullptr);
	
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, 
			details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
		&presentModeCount, details.presentModes.data());
	
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
			&presentModeCount, details.presentModes.data());
	}

	return details;
}

void Instance::AddDebugMessenger(DebugCallback* callback)
{
	if (!callback)
		return;

	for (uint64_t i = 0; i < debugCallbacks.size(); i++)
		if (debugCallbacks[i] == callback)
			return;
		
	debugCallbacks.push_back(callback);
}

void Instance::RemoveDebugMessenger(DebugCallback* callback)
{
	if (!callback)
		return;
	
	for (uint64_t i = 0; i < debugCallbacks.size(); i++)
		if (debugCallbacks[i] == callback)
		{
			debugCallbacks.erase(debugCallbacks.begin() + i);
			return;
		}
}

VkInstance* Instance::GetInstance()
{
	if (!initialized)
		return nullptr;
	
	return &instance;
}

void Instance::DispatchDebugMessage(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
)
{
	for (uint64_t i = 0; i < debugCallbacks.size(); i++)
		debugCallbacks[i]->OnDebugLog(messageSeverity, messageType,
			pCallbackData);
}

std::vector<VkExtensionProperties> Instance::GetAvailableExtentions()
{
	return extentions;
}

bool Instance::IsDebugMode()
{
	return debugMode;
}

void Instance::OnDispose()
{
	if (debugMode)
		Vulkan_DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	
	vkDestroyInstance(instance, nullptr);
}

bool Instance::IsDeviceSuitable(VkPhysicalDevice device, Surface* pSurface)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices families = FindQueueFamilies(device, pSurface);

	bool extentionsSupported = CheckDeviceExtentionSupport(device);

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		SwapchainDetails details = QuerySwapchainSupport(device, pSurface);
		swapChainGood = !details.formats.empty() 
			&& !details.presentModes.empty();
	}

	return 
		deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& families.hasGraphicsFamily
		&& families.hasPresentFamily
		&& swapChainGood
		&& extentionsSupported;
}

bool Instance::CheckDeviceExtentionSupport(VkPhysicalDevice device)
{
	// The device requires some extentions (such as the swap chain)
	// We need to check for those here before we can use the device.

	uint32_t extentionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount,
		nullptr);
	
	std::vector<VkExtensionProperties> availableExtentions(extentionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extentionCount,
		availableExtentions.data());
	
	std::set<std::string> requiredExtentions(deviceExtensions.begin(),
		deviceExtensions.end());
	
	for (VkExtensionProperties extention : availableExtentions)
		requiredExtentions.erase(extention.extensionName);
	
	return requiredExtentions.empty();
}

bool Instance::CheckValidationLayerSupport(std::vector<const char*> layers)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : layers) 
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
			if (std::strcmp(layerName, layerProperties.layerName) == 0) 
			{
				layerFound = true;
				break;
			}

		if (!layerFound)
			return false;
	}
	
	return true;
}

std::vector<const char*> Instance::GetRequiredExtentions(bool enableValidationLayers)
{
	// Vulkan needs extentions to interface with the window system.
	// GLFW has a built in function to get the extentions it needs.
	uint32_t glfwExtentionCount = 0;
	const char** glfwExtentions = glfwGetRequiredInstanceExtensions(
		&glfwExtentionCount
	);

	if (!glfwExtentions || glfwExtentionCount == 0)
		return std::vector<const char*>();

	std::vector<const char*> extentions(glfwExtentions, 
		glfwExtentions + glfwExtentionCount);
	
	if (enableValidationLayers)
		extentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extentions;
}