cmake_minimum_required (VERSION 3.8)
project ("VulkanHelloTriangle")

add_custom_target(GenerateResources
    ALL ld -r -b binary -o ../resources.o ../Assets/*.*
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/Main.cpp
)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c"
    "Headers/*.hpp"
    "Headers/*.h"
    "Assets/CompiledAssets/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)

target_link_libraries(${PROJECT_NAME}
    glfw vulkan dl pthread X11
)

target_link_options(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.o
)
