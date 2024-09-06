
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>

GLFWwindow* pWindow = NULL;
//窗口标题
const char* windowTitle = "Vulkan Window";

void TitleFps() {
	static double time0 = glfwGetTime();
	static double time1;
	static double dt;
	static int dframe = -1;
	static std::stringstream info;
	time1 = glfwGetTime();
	dframe++;
	if ((dt = time1 - time0) >= 1) {
		info.precision(1);
		info << windowTitle << "    " << std::fixed << dframe / dt << " FPS";
		glfwSetWindowTitle(pWindow, info.str().c_str());
		info.str("");//别忘了在设置完窗口标题后清空所用的stringstream
		time0 = time1;
		dframe = 0;
	}
}

int main()
{
	glfwInit();

	//GLFW_CLIENT_API的默认设置是GLFW_OPENGL_API，这种情况下，GLFW会在创建窗口时创建OpenGL的上下文，这对于Vulkan而言是多余的，所以向GLFW说明不需要OpenGL的API。
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	pWindow = glfwCreateWindow(1280, 720, windowTitle, nullptr, nullptr);


	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);


	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
		TitleFps();
	}

	glfwDestroyWindow(pWindow);

	glfwTerminate();

	return 0;
}