#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Image : public IDisposable
    {
    public:
        Image() {}
        Image(VkImage* pImage);
        
		Image(const Image&) = delete;
		Image(Image&&) = delete;
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) = delete;

        VkImage* GetImage();
    protected:
        void OnDispose();
    private:
        VkImage image;
        bool fromExisting = false;
    };
}