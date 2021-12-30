/*
 * An image view in Vulkan is.. well.. a thing that views the image.
 * Not quite, though. Image views in Vulkan describe how to access the image.
 */

#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Image.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/ImageViewDescriptor.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class ImageView : public IDisposable
    {
    public:
        /**
         * @brief Initializes the image view.
         * 
         * @param pDevice A pointer to the device this image view is being
         *  created for.
         * @param pImage A pointer to the image. 
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(Device* pDevice, Image* pImage, ImageViewDescriptor* pDesc);
        /**
         * @brief Initializes the image view.
         * 
         * @param pDevice A pointer to the device this image view is being
         *  created for.
         * @param pImage The vulkan image. 
         * 
         * @returns True if initialization was successful. Otherwise, false.
         */
        bool Init(Device* pDevice, VkImage image, ImageViewDescriptor* pDesc);

        VkImageView* GetImageView();
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;

        VkImageView imageView;
    };
}