/*
 * This file contains the code to run the application.
 * Main.cpp still runs the application.
 */

#pragma once

#include <iostream>
#include <vector>

#include <VulkanHelloTriangle/Graphics/Vulkan.hpp>

class Application
{
public:
    class DebugCallback : public Vulkan::DebugCallback
    {
    public:
        void OnDebugLog(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        );
    };

    void Start();
private:
    /**
     * @brief Initializes the window.
     * 
     * @param width The width of the window.
     * @param height The height of the window.
     * 
     * @return True, if the window was successfully initialized.
     *  Otherwise, false.
     */
    bool InitWindow(uint64_t width, uint64_t height);
    /**
     * @return True, if Vulkan was successfully initialized.
     *  Otherwise, false.
     */
    bool InitVulkan();
    bool InitDevice(VkPhysicalDevice physicalDevice, 
        Vulkan::QueueFamilyIndices indices);
    bool InitSwapchain(VkPhysicalDevice physicalDevice, 
        Vulkan::QueueFamilyIndices indices);
    bool InitShaders();
    bool InitRenderPass();
    bool InitPipeline();
    bool InitFramebuffers();
    bool InitCommandPool(Vulkan::QueueFamilyIndices indices);
    bool InitSemaphores();

    void Render();
    void Present();

    /**
     * @brief Cleans up the application.
     */
    void Dispose();

    GLFWwindow* window = nullptr;
    Vulkan::Instance vulkan;
    Vulkan::Surface surface;
    Vulkan::Device device;
    Vulkan::Queue graphicsQueue;
    Vulkan::Queue presentQueue;
    Vulkan::Swapchain swapchain;
    Vulkan::PipelineLayout pipelineLayout;
    Vulkan::RenderPass renderPass;
    Vulkan::Pipeline pipeline;
    Vulkan::CommandPool commandPool;

    // Shader stuff
    Vulkan::ShaderModule vertexModule;
    Vulkan::ShaderModule fragmentModule;

    Vulkan::Semaphore imageAvailableSemaphore;
    Vulkan::Semaphore renderFinishedSemaphore;
    
    std::vector<Ref<Vulkan::Framebuffer>> framebuffers;
    std::vector<Ref<Vulkan::ImageView>> swapchainImageViews;
    std::vector<Vulkan::CommandBuffer> commandBuffers;

    DebugCallback callback;
};