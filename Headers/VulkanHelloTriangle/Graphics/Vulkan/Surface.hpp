/*
 * This header defines the surface. Surfaces in Vulkan are interfaces with
 * the window.
 */

#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Vulkan
{
    class Instance;
    class Surface : public IDisposable
    {
    public:
        /**
         * @brief Initializes the surface object.
         * 
         * @param pInstance A pointer to the Vulkan instance.
         * @param window The GLFW window object.
         * 
         * @returns True if initialization succeeded. Otherwise, false.
         */
        bool Init(Instance* pInstance, GLFWwindow* window);

        VkSurfaceKHR* GetSurface();
    protected:
        void OnDispose();
    private:
        Instance* pInstance;
        VkSurfaceKHR surface;
    };
}