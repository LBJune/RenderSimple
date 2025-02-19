cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

# 收集所有的源文件和头文件
file(GLOB_RECURSE CORE_SOURCES 
    "${CURRENT_DIR}/core/*.cpp"
    "${CURRENT_DIR}/core/*.h"
)

file(GLOB_RECURSE UTILS_SOURCES 
    "${CURRENT_DIR}/utils/*.cpp"
    "${CURRENT_DIR}/utils/*.h"
)

# 设置当前 CMake 目录
include_directories(${CURRENT_DIR})

# 创建筛选器
source_group("core" FILES ${CORE_SOURCES})
source_group("utils" FILES ${UTILS_SOURCES})

# 添加可执行文件
add_library(VKit ${CORE_SOURCES} ${UTILS_SOURCES})

find_package(Vulkan REQUIRED)

include(FetchContent)
FetchContent_Declare(spirv_cross
                    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
                    GIT_TAG vulkan-sdk-1.4.304
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/spirv_cross)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "Disable tests")
set(SPIRV_CROSS_CLI OFF CACHE BOOL "Disable CLI")

FetchContent_Declare(glslang
                    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
                    GIT_TAG vulkan-sdk-1.4.304
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glslang)
set(BUILD_EXTERNAL OFF CACHE BOOL "Disable OSS-Fuzz")
set(ENABLE_OPT OFF CACHE BOOL "Disable optimizer")

FetchContent_MakeAvailable(spirv_cross glslang)

target_link_libraries(VKit
    PRIVATE 
    Vulkan::Vulkan 
    glslang::glslang  # GLSL编译器
    glslang::glslang-default-resource-limits
    glslang::SPIRV       # SPIR-V生成
    spirv-cross-glsl
)

target_include_directories(VKit PRIVATE Vulkan::Vulkan  ${spirv_cross_SOURCE_DIR} ${glslang_SOURCE_DIR})

# 设置库的头文件目录
target_include_directories(VKit PUBLIC 
    ${CURRENT_DIR}  # 确保包含目录是公开的
)