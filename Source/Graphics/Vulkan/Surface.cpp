#include <VulkanHelloTriangle/Graphics/Vulkan/Surface.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Instance.hpp>

using namespace Vulkan;

bool Surface::Init(Instance* pInstance, GLFWwindow* window)
{
    if (initialized)
        return false;
    
    this->pInstance = pInstance;

    if (glfwCreateWindowSurface(
            pInstance->Get(),
            window,
            nullptr,
            &surface
        ) != VK_SUCCESS)
        return false;
    
    initialized = true;
    return true;
}

VkSurfaceKHR Surface::Get()
{
    return surface;
}

void Surface::OnDispose()
{
    vkDestroySurfaceKHR(pInstance->Get(), surface, nullptr);
}