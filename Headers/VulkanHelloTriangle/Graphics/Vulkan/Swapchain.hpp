/*
 * The swap chain is a queue of images to present to the screen.
 */

#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Common/Ref.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Surface.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Image.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/ImageView.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/SwapchainDescriptor.hpp>

#include <vulkan/vulkan.hpp>

namespace Vulkan
{
    class Swapchain : public IDisposable
    {
    public:
        Swapchain() {}

		Swapchain(const Swapchain&) = delete;
		Swapchain(Swapchain&&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		Swapchain& operator=(Swapchain&&) = delete;

        /**
         * @brief Initializes the swapchain.
         * 
         * @param pSurface A pointer to the surface object to tie the
         *  swapchain to.
         * @param pDevice A pointer to the device to the the swapchain to.
         * @param swapchainDesc A pointer to a SwapchainDescriptor struct
         *  describing how to set up the swapchain.
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(
            Surface* pSurface,
            Device* pDevice,
            uint64_t width, uint64_t height,
            SwapchainDescriptor* swapchainDesc
        );

        /**
         * @brief Resizes the swapchain images to match the specified width and
         *  height.
         * 
         * @warning THIS FUNCTION IS CURRENTLY UNIMPLEMENTED
         * 
         * @param width The new width of the swapchain images.
         * @param height The new height of the swapchain images.
         * 
         * @returns True if the resize was successful. Otherwise, false.
         */
        bool ResizeBuffers(uint64_t width, uint64_t height);

        VkSurfaceFormatKHR ChooseSurfaceFormat(SwapchainDetails details);

        uint32_t GetImageCount();
        std::vector<Ref<Image>> GetImages();
        std::vector<Ref<ImageView>> GetImageViews();

        VkExtent2D GetExtent();
        VkFormat GetImageFormat();
        uint64_t GetWidth();
        uint64_t GetHeight();

        VkSwapchainKHR* GetSwapchain();
    protected:
        void OnDispose();
    private:
        VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR>& presentModes);
        VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR& capabilities,
            uint64_t width, uint64_t height);

        uint32_t imageCount = 0;
        VkSwapchainKHR swapchain;

        VkExtent2D extent;
        VkFormat imageFormat;

        uint64_t width = 0, height = 0;

        Device* pDevice = nullptr;
    };
}