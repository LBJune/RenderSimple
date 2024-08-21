cmake_minimum_required(VERSION 3.12.0)

project(RenderSimple VERSION 1.0.0)

add_executable(app main.cpp)

include(FetchContent)
FetchContent_Declare(glfw
                    GIT_REPOSITORY https://github.com/glfw/glfw.git
                    GIT_TAG 3.4
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glad
                    GIT_REPOSITORY https://github.com/skaarj1989/glad.git
                    GIT_TAG gl
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad)
FetchContent_MakeAvailable(glad)

target_link_libraries(app PRIVATE glfw glad)

