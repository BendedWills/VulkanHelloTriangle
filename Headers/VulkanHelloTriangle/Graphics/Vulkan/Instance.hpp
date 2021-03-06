/*
 * This header provides the functions to use Vulkan in the application.
 */

#pragma once

#include <vector>
#include <unordered_map>

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/DebugCallback.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Surface.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/SwapchainDetails.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/QueueFamilyIndices.hpp>

#include <GLFW/glfw3.h>

namespace Vulkan
{
	class Instance : public IDisposable
	{
	public:
		Instance() {}

		Instance(const Instance&) = delete;
		Instance(Instance&&) = delete;
		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&) = delete;

		/**
		 * @brief Initializes Vulkan.
		 * 
		 * @param applicationName The name of the application.
		 * @param engineName The name of the engine.
		 * @param debugMode True if the Vulkan instance should be in
		 *  debug mode.
		 * 
		 * @returns True if initialization succeeded. Otherwise, false.
		 */
		bool Init(
			const char* applicationName = "VulkanApp",
			const char* engineName = "VulkanEngine",
			bool debugMode = false
		);

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device,
			Surface* pSurface);
		SwapchainDetails QuerySwapchainSupport(VkPhysicalDevice device, 
			Surface* pSurface);
		bool CheckDeviceExtentionSupport(VkPhysicalDevice device, 
			const char*const * deviceExtentions, uint64_t extentionCount);
		
		/**
		 * @brief Adds a debug messenger.
		 * 
		 * @param messenger The debug messenger.
		 */
		void AddDebugMessenger(DebugCallback* messenger);
		/**
		 * @brief Removes a debug messenger.
		 * 
		 * @param messenger The debug messenger.
		 */
		void RemoveDebugMessenger(DebugCallback* messenger);

		VkInstance Get();
		std::vector<VkExtensionProperties> GetAvailableExtentions();

		void DispatchDebugMessage(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
		);

		bool IsDebugMode();

		// Validation layers check when errors happen in Vulkan.
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	protected:
		void OnDispose();
	private:
		bool CheckValidationLayerSupport(std::vector<const char*> layers);
		std::vector<const char*> GetRequiredExtentions(bool debugMode);

		// The Vulkan instance. Essentially, it is the connection between this
		// application and Vulkan.
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		
		std::vector<DebugCallback*> debugCallbacks;
		std::vector<VkExtensionProperties> extentions;

		bool debugMode = false;
	};
}
