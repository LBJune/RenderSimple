cmake_minimum_required(VERSION 3.12.0)

project(VKit VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(glfw
                    GIT_REPOSITORY https://github.com/glfw/glfw.git
                    GIT_TAG 3.4
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
FetchContent_MakeAvailable(glfw)

add_subdirectory(vkit)
add_subdirectory(samples/hello_triangle)
