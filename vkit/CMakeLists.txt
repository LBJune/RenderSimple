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
target_link_libraries(VKit PRIVATE Vulkan::Vulkan)

target_include_directories(VKit PRIVATE Vulkan::Vulkan)

# 设置库的头文件目录
target_include_directories(VKit PUBLIC 
    ${CURRENT_DIR}  # 确保包含目录是公开的
)
