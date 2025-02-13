#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <core/instance.h>
#include <core/physical_device.h>
#include <core/device.h>
#include <core/swapchain.h>
#include <core/image.h>
#include <core/image_view.h>

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
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
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
