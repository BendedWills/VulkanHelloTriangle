cmake_minimum_required (VERSION 3.8)
project ("VulkanHelloTriangle")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)

# target_link_directories(${PROJECT_NAME}
# )
