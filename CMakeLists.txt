cmake_minimum_required (VERSION 3.8)
project ("VulkanHelloTriangle")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DDEBUG_ENABLED)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

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
