#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class Image : public IDisposable
    {
    public:
        Image() {}

        VkImage* GetImage();
    protected:
        Image(VkImage* pImage);

        void OnDispose();
    private:
        VkImage image;
    };
}