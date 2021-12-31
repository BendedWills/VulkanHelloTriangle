#include <VulkanHelloTriangle/Application.hpp>
#include <VulkanHelloTriangle/Common/Resource.hpp>
#include <set>
#include <string>

void Application::DebugCallback::OnDebugLog(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
}

void Application::Start()
{
    if (!InitWindow(1280, 720))
    {
        std::cout << "Failed to initialize window!" << std::endl;
        return;
    }

    if (!InitVulkan())
    {
        glfwDestroyWindow(window);
        std::cout << "Failed to initialize Vulkan!" << std::endl;
        return;
    }

    std::cout << "Started application" << std::endl;

    // glfwWindowShouldClose returns if the user has closed the window
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        Render();
    }

    Dispose();
    std::cout << "User closed applocation" << std::endl;
}

bool Application::InitWindow(uint64_t width, uint64_t height)
{
    // The GLFW_CLIENT_API hint in short tells GLFW to not use OpenGL.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, "Vulkan Hello Triangle", nullptr, 
        nullptr);
    if (!window)
        return false;
    
    Resource solidFragResource = LOAD_RESOURCE(___Assets_solidFrag_glsl);
    std::cout << std::string((const char*)solidFragResource.GetData(), 
        solidFragResource.GetSize()) << std::endl;
    
    return true;
}

bool Application::InitVulkan()
{
    vulkan.AddDebugMessenger(&callback);
    if (!vulkan.Init(nullptr, nullptr, true))
        return false;
    
    if (!surface.Init(&vulkan, window))
        return false;
    
    VkPhysicalDevice physicalDevice;
    if (!vulkan.PickDevice(&physicalDevice, &surface))
        return false;
    
    Vulkan::QueueFamilyIndices indices = 
        vulkan.FindQueueFamilies(physicalDevice, &surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamilyIndex,
        indices.presentFamilyIndex
    };
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures features{};

    if (!device.Init(
            &vulkan, 
            physicalDevice, 
            queueCreateInfos.data(), 
            queueCreateInfos.size(), 
            features, 
            vulkan.deviceExtensions.data(),
            vulkan.deviceExtensions.size()
        ))
        return false;
    
    graphicsQueue = device.GetDeviceQueue(indices.graphicsFamilyIndex, 0);
    presentQueue = device.GetDeviceQueue(indices.presentFamilyIndex, 0);

    Vulkan::SwapchainDetails details = 
        vulkan.QuerySwapchainSupport(physicalDevice, &surface);

    VkSurfaceFormatKHR surfaceFormat = swapchain.ChooseSurfaceFormat(
        details);

    Vulkan::SwapchainDescriptor swapchainDesc;
    swapchainDesc.imageFormat = surfaceFormat.format;
    swapchainDesc.imageColorSpace = surfaceFormat.colorSpace;
    swapchainDesc.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    swapchainDesc.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (indices.graphicsFamilyIndex != indices.presentFamilyIndex)
        swapchainDesc.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    
    swapchainDesc.preTransform = details.capabilities.currentTransform;
    swapchainDesc.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainDesc.familyIndices = indices;
    swapchainDesc.details = details;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (!swapchain.Init(&surface, &device, width, height, &swapchainDesc))
        return false;
    
    return true;
}

void Application::Render()
{
    
}

void Application::Dispose()
{
    swapchain.Dispose();
    device.Dispose();
    surface.Dispose();
    vulkan.Dispose();
    glfwDestroyWindow(window);
}
