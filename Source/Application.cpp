#include <VulkanHelloTriangle/Application.hpp>

void Application::Start()
{
    // Create the window
    // The GLFW_CLIENT_API hint in short tells GLFW to not use OpenGL.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, "Vulkan Hello Triangle", nullptr, nullptr);

    uint32_t extentionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extentionCount, nullptr);

    std::cout << extentionCount << " extentions supported" << std::endl;

    // glfwWindowShouldClose returns if the user has closed the window
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        Render();
    }

    std::cout << "User closed applocation" << std::endl;
    Dispose();
}

void Application::Render()
{
    
}

void Application::Dispose()
{
    glfwDestroyWindow(window);
}