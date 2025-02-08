cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/hello_triangle.cpp"
)

include_directories (${VKit_INCLUDE_DIRS})

add_executable(hello_triangle ${SOURCES})
target_link_libraries(hello_triangle PRIVATE glfw VKit)

find_package(Vulkan REQUIRED)
target_include_directories(hello_triangle PRIVATE Vulkan::Vulkan)
target_link_libraries(hello_triangle PRIVATE Vulkan::Vulkan)