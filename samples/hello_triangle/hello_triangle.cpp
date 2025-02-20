#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#define NOMINMAX
#include <windows.h>

#include <core/instance.h>
#include <core/physical_device.h>
#include <core/device.h>
#include <core/swapchain.h>
#include <core/image.h>
#include <core/image_view.h>
#include <core/shader_module.h>
#include <core/pipeline.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window;
	//窗口标题
	const char* windowTitle = "Hello Triangle";

	std::shared_ptr<vkit::Instance> instance;
	std::shared_ptr<vkit::Device> device;

	VkSurfaceKHR surface;
	std::shared_ptr<vkit::Swapchain> swapchain;
	std::vector<vkit::Image*> images;
	std::vector<vkit::ImageView*> imageviews;

	std::vector<vkit::ShaderModule *> shader_modules;
	std::shared_ptr<vkit::GraphicsPipeline> pipeline;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	void initWindow() {
		glfwInit();

		//GLFW_CLIENT_API的默认设置是GLFW_OPENGL_API，这种情况下，GLFW会在创建窗口时创建OpenGL的上下文，这对于Vulkan而言是多余的，所以向GLFW说明不需要OpenGL的API。
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, windowTitle, nullptr, nullptr);
	}

	void initVulkan() {
		instance = std::shared_ptr<vkit::Instance>(new vkit::Instance(windowTitle, getRequiredExtensions()));
		createSurface();
		device = std::shared_ptr<vkit::Device>(new vkit::Device(instance->get_suitable_gpu(surface), surface, deviceExtensions));
		swapchain = std::shared_ptr<vkit::Swapchain>(new vkit::Swapchain(*device.get(), surface));
		std::vector<VkImage> imgs = swapchain->get_images();
		for (auto image : imgs)
		{
			vkit::Image* newImage = new vkit::Image(*device.get(), image, VkExtent3D{ swapchain->get_extent().width, swapchain->get_extent().height, 1 }, swapchain->get_format(), swapchain->get_usage());
			images.push_back(newImage);
			imageviews.push_back(new vkit::ImageView(*newImage, VK_IMAGE_VIEW_TYPE_2D));
		}

		shader_modules.push_back(new vkit::ShaderModule(*device.get(), VK_SHADER_STAGE_VERTEX_BIT, vkit::ShaderSource(getExeDirectory() + "\\shader\\shader_base.vert"), std::string("main"), {}));
		shader_modules.push_back(new vkit::ShaderModule(*device.get(), VK_SHADER_STAGE_FRAGMENT_BIT, vkit::ShaderSource(getExeDirectory() + "\\shader\\shader_base.frag"), std::string("main"), {}));

		pipeline = std::shared_ptr<vkit::GraphicsPipeline>(new vkit::GraphicsPipeline(*device.get(), shader_modules));
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {

		pipeline.reset();

		for (auto shader_module : shader_modules)
		{
			delete shader_module;
		}

		for (auto imageview : imageviews)
		{
			delete imageview;
		}

		for (auto image : images)
		{
			delete image;
		}
		swapchain.reset();
		vkDestroySurfaceKHR(instance->get_handle(), surface, nullptr);
		device.reset();
		instance.reset();

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	//调用需要的扩展，否则surface会创建失败
	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance->get_handle(), window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	std::string getExeDirectory() {
		char path[MAX_PATH];
		GetModuleFileNameA(nullptr, path, MAX_PATH);
		std::string exePath = path;
		return exePath.substr(0, exePath.find_last_of("\\/"));
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
