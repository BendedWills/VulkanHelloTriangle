cmake_minimum_required (VERSION 3.8)
project ("VulkanHelloTriangle")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c"
    "Headers/*.hpp"
    "Headers/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)

target_link_libraries(${PROJECT_NAME}
    glfw vulkan dl pthread X11
)
