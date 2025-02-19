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

set(BIN_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
set_target_properties(hello_triangle PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BIN_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${BIN_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL  "${BIN_PATH}"
)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shader)

file(GLOB SHADER_FILES ${SHADER_DIR}/*)

# 将 shader 文件拷贝到可执行路径
file(COPY ${SHADER_FILES} DESTINATION "${BIN_PATH}/shader")