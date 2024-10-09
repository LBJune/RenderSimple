cmake_minimum_required(VERSION 3.12.0)

project(RenderSimple VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(app main.cpp)

include(FetchContent)
FetchContent_Declare(glfw
                    GIT_REPOSITORY https://github.com/glfw/glfw.git
                    GIT_TAG 3.4
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm
                    GIT_REPOSITORY https://github.com/g-truc/glm.git
                    GIT_TAG 1.0.1
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm)

FetchContent_MakeAvailable(glm)


find_package(Vulkan REQUIRED)
target_include_directories(app PRIVATE Vulkan::Vulkan)

target_link_libraries(app PRIVATE glfw glm Vulkan::Vulkan)

set(BIN_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
set_target_properties(app PROPERTIES
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