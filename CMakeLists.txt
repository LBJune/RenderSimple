cmake_minimum_required(VERSION 3.12.0)

project(RenderSimple VERSION 1.0.0)

add_executable(app main.cpp)

include(FetchContent)
FetchContent_Declare(glfw
                    GIT_REPOSITORY https://github.com/glfw/glfw.git
                    GIT_TAG 3.4
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
FetchContent_MakeAvailable(glfw)

find_package(Vulkan REQUIRED)
target_include_directories(app PRIVATE Vulkan::Vulkan)

target_link_libraries(app PRIVATE glfw Vulkan::Vulkan)