#include <VulkanHelloTriangle/Application.hpp>

int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return 1;
    }

    Application app;
    app.Start();

    glfwTerminate();

    return 0;
}