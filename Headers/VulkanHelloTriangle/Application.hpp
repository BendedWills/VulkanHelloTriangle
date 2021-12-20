/*
 * This file contains the code to run the application.
 * Main.cpp still runs the application.
 */

#pragma once

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Application
{
public:
    void Start();
private:
    void Render();

    /**
     * @brief Cleans up the application.
     */
    void Dispose();

    GLFWwindow* window = nullptr;
};