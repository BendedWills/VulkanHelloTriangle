cmake_minimum_required (VERSION 3.8)
project ("VulkanHelloTriangle")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c"
    "Headers/*.hpp"
    "Headers/*.h"
)

add_definitions(-DSHADER_COMPILATION)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)

target_link_libraries(${PROJECT_NAME}
    glfw vulkan dl pthread X11 glslang SPIRV OSDependent
)

file(GLOB_RECURSE RESOURCES 
    "Assets/*.o"
)

target_link_options(${PROJECT_NAME} 
    PUBLIC
    ${RESOURCES}
)
