/*
 * An image view in Vulkan is.. well.. a thing that views the image.
 * Not quite, though. Image views in Vulkan describe how to access the image.
 */

#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class ImageView : public IDisposable
    {
    public:
        bool Init(Device* pDevice, );

        VkImageView* GetImageView();
    protected:
        void OnDispose();
    private:
        Device* pDevice = nullptr;

        VkImageView imageView;
    };
}