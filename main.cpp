
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


GLFWwindow* pWindow = NULL;
//窗口标题
const char* windowTitle = "Vulkan Window";

VkInstance instance = VK_NULL_HANDLE;

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkDevice device;
VkQueue graphicsQueue;
VkQueue presentQueue;

VkSurfaceKHR surface;
VkSwapchainKHR swapChain;
std::vector<VkImage> swapChainImages;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

std::vector<VkImageView> swapChainImageViews;

VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;

std::vector<VkFramebuffer> swapChainFramebuffers;

VkCommandPool commandPool;
//VkCommandBuffer commandBuffer;
std::vector<VkCommandBuffer> commandBuffers;

//VkSemaphore imageAvailableSemaphore;
//VkSemaphore renderFinishedSemaphore;
//VkFence inFlightFence;

std::vector<VkSemaphore> imageAvailableSemaphores;
std::vector<VkSemaphore> renderFinishedSemaphores;
std::vector<VkFence> inFlightFences;

const int MAX_FRAMES_IN_FLIGHT = 2;
uint32_t currentFrame = 0;

bool framebufferResized = false;

VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;

VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;
std::vector<VkDescriptorSet> descriptorSets;

std::vector<VkBuffer> uniformBuffers;
std::vector<VkDeviceMemory> uniformBuffersMemory;
std::vector<void*> uniformBuffersMapped;

VkImage textureImage;
VkDeviceMemory textureImageMemory;

//VK_LAYER_NV_optimus：NVIDIA 提供的优化层，主要是用于识别基于 Vulkan 等的游戏，并切换为独立 GPU 来提供更好的性能。
//VK_LAYER_OBS_HOOK：用于支持 Open Broadcaster Software(OBS 等游戏直播、录屏软件的接口)。
//VK_LAYER_RENDERDOC_Capture：RenderDoc 提供的抓帧功能层，可以在特定的时刻捕获渲染的帧状态，用于之后的离线分析。
//VK_LAYER_EOS_Overlay：用于 Epic Online Service （Epic 游戏在线服务）的覆盖层。
//VK_LAYER_INTEL_state_tracker：Intel 提供的状态追踪层，用于分析与 Intel GPU 相关的问题。
//VK_LAYER_LUNARG_api_dump： LunarG 提供的 API 调用抓取层，它可以记录所有的 Vulkan API 调用信息。
//VK_LAYER_LUNARG_gfxreconstruct：用于捕获和回放Vulkan API调用，以帮助重现和调试问题。
//VK_LAYER_KHRONOS_synchronization2：对 Vulkan 同步操作的改进，提供更好的同步功能。
//VK_LAYER_KHRONOS_validation：由 Khronos 团队为 Vulkan API 提供的验证层，用于开发时追踪 API 的调用错误，并给出有关如何修复的提示。
//VK_LAYER_LUNARG_monitor：用于监视 Vulkan 应用程序的执行信息。
//VK_LAYER_LUNARG_screenshot：用于捕获 Vulkan 渲染帧的层。
//VK_LAYER_KHRONOS_profiles：用于分析和优化 Vulkan 应用的层。
//VK_LAYER_KHRONOS_shader_object：用于管理和调试 Vulkan 着色器对象。
//VK_LAYER_LUNARG_crash_diagnostic：用于在程序崩溃时生成诊断报告。
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//Vulkan API提供的错误检查功能非常有限,需要显式地定义每一个操作，用于开发者更好地理解 Vulkan API 调用的行为，意味着在开发阶段可以启用校验层进行详尽的错误检测和性能建议，在发布阶段则可以禁用，以避免额外的性能开销
//作用：检测参数值是否合法 ；追踪对象的创建和清除操作，发现资源泄漏问题 ； 追踪调用来自的线程，检测是否线程安全； 将API调用和调用的参数写入日志 追踪API调用进行分析和回放
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkDebugUtilsMessengerEXT debugMessenger;

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

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

std::string getExeDirectory() {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string exePath = path;
	return exePath.substr(0, exePath.find_last_of("\\/"));
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	framebufferResized = true;
}

//调试信息的回调，参考原型 PFN_vkDebugUtilsMessengerCallbackEXT
//参数messageSeverity：
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT：诊断信息
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT：	资源创建之类的信息
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT	：警告信息
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT	：错误信息(不合法和可能造成崩溃的操作)
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT	//枚举的最大参数， 未有直接的使用价值
//参数messageType：
//			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: 发生了一些与规范和性能无关的事件
//			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT : 出现了违反规范的情况或发生了一个可能的错误
//			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT : 进行了可能影响Vulkan性能的行为
//参数pCallbackData	: VkDebugUtilsMessengerCallbackDataEXT 
//			pMessage: 一个以null结尾的包含调试信息的字符串
//			pObjects : 存储有和消息相关的Vulkan对象句柄的数组
//			objectCount : 数组中的对象个数
//参数pUserData：指向了设置回调函数时，传递的数据的指针

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	//指定回调函数处理的消息级别
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	//指定回调函数处理的消息类型
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	//回调函数的指针
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // Optional
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void createSurface() {
	if (glfwCreateWindowSurface(instance, pWindow, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void checValidationExtensionSupport()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:\n";

	for (const auto& extension : extensions) {
		std::cout << '\t' << extension.extensionName << '\n';
	}
}

std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		//使用VK_EXT_debug_utils扩展，设置回调函数来接受调试信息。
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);	//To set up a callback in the program to handle messages and the associated details, we have to set up a debug messenger with a callback using the VK_EXT_debug_utils extension.
	}

	return extensions;
}

bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::cout << "available Layers:\n";

	for (const auto& layerProperties : availableLayers) {
		std::cout << '\t' << layerProperties.layerName << '\n';
	}

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}


void createInstance() {
	checValidationExtensionSupport();//检查扩展

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;	//Vulkan API版本为1.0

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();

	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();	//开启的扩展层名称

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();	//开启的校验层名称

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;	//仅用于实例创建和销毁的时候，返回调试信息
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {	//创建vulkan实例
		throw std::runtime_error("failed to create instance!");
	}
}


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		//VK_QUEUE_GRAPHICS_BIT 意味队列族支持图形操作，且可以执行包括顶点、几何、片段着色器等在内的图形命令
		//其他
		//VK_QUEUE_COMPUTE_BIT 队列支持计算操作（计算着色器）
		//VK_QUEUE_TRANSFER_BIT 队列支持传输操作(例如：内存拷贝操作)
		//VK_QUEUE_SPARSE_BINDING_BIT 队列支持稀疏资源内存绑定操作
		//VK_QUEUE_PROTECTED_BIT 队列支持受保护操作(受保护的命令在执行时，其数据和操作对应用的其余部分是不可访问的，包括CPU以及其他的GPU执行单元)。可用于保护敏感数据，防止它们被未授权的应用访问，特别在如视频流解码等场景中，需要确保解码数据在应用中的其余部分是不可见的
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);	//查询给定sueface在物理设备上的基本能力，如尺寸、图像数量、图像数组大小

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);	//查询给定sueface支持的格式数量，

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data()); //查询给定sueface支持的格式
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);	//查询给定的物理设备到surface的有效呈现模式

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());	//查询给定的物理设备到surface的有效呈现模式
	}
	return details;
}

//选择符合要求的设备
bool isDeviceSuitable(VkPhysicalDevice device) {
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);//基础的设备属性，比如名称，类型和支持的Vulkan版本

	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);//查询功能特性，比如纹理压缩，64位浮点和多视口渲染(常用于VR)等特性的支持

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;

}

void pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());//获取物理设备

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		   indices.graphicsFamily.value(),
		   indices.presentFamily.value()
	};
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;//这是一个指向浮点数数组的指针，用于设定一组设备队列的优先级。这些优先级被用于在系统中的多个并行队列中调度命令的执行。指定的优先级必须在0.0到1.0之间，包括0.0和1.0。其中，1.0是最高优先级。
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {	//逻辑设备并不直接与Vulkan实例交互，所以创建逻辑设备时不需要使用Vulkan实例作为参数
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

//VK_PRESENT_MODE_IMMEDIATE_KHR			应用程序提交的图像会立即传输到屏幕上，这可能会导致画面撕裂。
//VK_PRESENT_MODE_FIFO_KHR				FIFO（First In First Out），交换链是个队列，当队列满了，程序就会停下来。这与现代屏幕的垂直同步最接近，可以避免撕裂。
//VK_PRESENT_MODE_FIFO_RELAXED_KHR		只有当帧在下一次垂直刷新之前完成更新，才进行帧刷新。如果帧晚了，那么就直接交换，可能会导致撕裂。和FIFO的区别在于尚未完成渲染的帧的处理。FIFO是会延用旧画面，而FIFO_RELAXED会先使用旧画面，当新帧完成后，会立即交换，这样会出现撕裂。
//VK_PRESENT_MODE_MAILBOX_KHR			这是第二种模式的另一种变体。队列已满时，应用程序不会阻塞，而是用较新的图像替换已排队的图像。此模式可用于尽可能快地渲染帧，同时仍避免撕裂，从而比标准垂直同步减少延迟问题。这通常称为“三重缓冲”，尽管仅存在三个缓冲区并不一定意味着帧速率已解锁。
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	uint32_t max_uint32 = std::numeric_limits<uint32_t>::max();
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(pWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void createSwapChain() {
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//设置交换链的属性
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;	//VK_SHARING_MODE_CONCURRENT：图像可以在多个队列系列中使用，而无需明确转让所有权。
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;	//使用VK_SHARING_MODE_CONCURRENT需要指定要使用的队列家族的数目和列表
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//VK_SHARING_MODE_EXCLUSIVE：一个图像一次只能由一个队列系列拥有，在另一个队列系列中使用它之前必须明确转让所有权。此选项提供最佳性能。
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;	//不透明，忽略alpha
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}


void createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;	//指定图像视图类型为 1D 纹理、2D 纹理、3D 纹理和立方体贴图
		createInfo.format = swapChainImageFormat;

		//为每个颜色通道（R，G，B，A）重新映射颜色值
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;	//不改变该通道（默认）
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//subresourceRange成员变量用于指定图像的用途和图像的哪一部分可以被访问
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;	//确定图像视图是色彩图像、深度模板等还是都包括
		createInfo.subresourceRange.baseMipLevel = 0;	//指定访问的mipmap层次的范围开始（base）和个数（count）
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;	//指定访问的数组层的范围开始（base）和个数（count）
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;	//和多重采样相关

	// VK_ATTACHMENT_LOAD_OP_LOAD：保持附着的现有内容 
	// VK_ATTACHMENT_LOAD_OP_CLEAR：使用一个常量值来清除附着的内容 
	// VK_ATTACHMENT_LOAD_OP_DONT_CARE：不关心附着现存的内容
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//VK_ATTACHMENT_STORE_OP_STORE：渲染的内容会被存储起来，以便之后读取 
	//VK_ATTACHMENT_STORE_OP_DONT_CARE：渲染后，不会读取帧缓冲的内容
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	//渲染流程开始前的图像布局方式,VK IMAGE LAYOUT UNDEFINED表示我们不关心之前的图像布局方式
	// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL：图像被用作颜色附着 
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR：图像被用在交换链中进行呈现操作 
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL：图像被用作复制操作的目的图像
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	//指定渲染流程结束后的图像布局方式

	VkAttachmentReference colorAttachmentRef{};	//渲染过程中引用给定的附件
	colorAttachmentRef.attachment = 0;		//指定附件在渲染过程的附件描述符数组中的索引，即上面colorAttachment的索引
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	//图像被用作颜色附着 

	//子渲染过程
	VkSubpassDescription subpass{};	
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;	//指定渲染过程是图形渲染（VK_PIPELINE_BIND_POINT_GRAPHICS）或计算渲染（VK_PIPELINE_BIND_POINT_COMPUTE）。
	subpass.colorAttachmentCount = 1;	//颜色附件的数量
	subpass.pColorAttachments = &colorAttachmentRef;	//颜色附件的指针

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

VkShaderModule createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

std::vector<float> vertices =
{
	//---位置---//  //---颜色---//
	-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	//左上	与opengl在y轴上相反
	 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,	//右上
	-0.5f,  0.5f,  0.0f, 0.0f, 1.0f,	//左下
	 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,	//右下
};

std::vector <int16_t> indices = {
	0, 1, 2, // 第一个三角形
	1, 3, 2  // 第二个三角形
};

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;	//图像类型（1D，2D或3D）
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;	//规定像素如何在内存中排列，可选包括 VK_IMAGE_TILING_LINEAR 和 VK_IMAGE_TILING_OPTIMAL，即线性排序和优化排序
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	//初始化布局，VK_IMAGE_LAYOUT_UNDEFINED：通常用于新创建的图像，因为此时我们通常不关心图像的现有内容
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	//VK_SHARING_MODE_EXCLUSIVE: 在任何给定时间，资源只能在一个队列家族中使用。如果资源需要在多个队列家族中使用，必须显式执行所有权传递。这种模式在性能上可能更优，因为允许进行某些硬件优化。
	//VK_SHARING_MODE_CONCURRENT: 在多个队列家族之间共享资源，不需要明确的所有权传递。如果你在多个队列之间频繁共享资源，可能会希望使用这种模式。
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional	图像创建标识，如立方体贴图(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)、3D等，参考枚举VkImageCreateFlagBits	 

	if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, textureImage, textureImageMemory, 0);
}

void createTextureImage() {
	std::string path = getExeDirectory() + "\\image\\picture.png";
	//stbi_set_flip_vertically_on_load(true);

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	stbi_image_free(pixels);

}

//在 Vulkan 中，描述符集（`VkDescriptorSet`）是一个向着色器程序提供输入（如：缓冲器，图像等）的方式。它具体地指定了每个描述符引用的资源（比如：缓冲区，图像等）在 GPU 内存中的细节。可以把一个描述符集看作是一组GPU资源的引用集合。
//
//在着色器中，描述符通常以变量的形式和一个特定的绑定点一起使用，允许在渲染过程中在 GPU 内存中访问数据。描述符通过描述符布局（`VkDescriptorSetLayout`）和描述符池（`VkDescriptorPool`）来管理。
//
//当绘制命令提交给执行队列以执行渲染任务时，可以绑定一个或多个`VkDescriptorSet`到显卡的绑定点上，从而使着色器能够访问这些绑定点所引用的GPU资源。
void createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};	//用于描述着色器程序中单个绑定点的描述符
    uboLayoutBinding.binding = 0;	//binding point
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;	//描述符类型，决定了着色器程序如何访问该数据。例如：VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER，VK_DESCRIPTOR_TYPE_STORAGE_BUFFER，VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE等
    uboLayoutBinding.descriptorCount = 1;	//对于数组类型的绑定，这指定了数组长度。对于非数组类型的绑定，该值通常是1
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;	//表示这个绑定点会被哪些着色器阶段使用，可以是多个阶段的组合，例如：VK_SHADER_STAGE_VERTEX_BIT，VK_SHADER_STAGE_FRAGMENT_BIT。

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

//在Vulkan中，描述符池（VkDescriptorPool）是用于分配描述符集(VkDescriptorSet)的对象，担当着Vulkan的内存管理器的角色。
//
//创建一个新的VkDescriptorSet时，需要从一个VkDescriptorPool对象中分配。每个VkDescriptorPool可以设置一个最大限制，限制了它可以分配的描述符的类型和数量。
//
//描述符池的职责并非只是分配和管理描述符。当你销毁或重置一个描述符池时，由该池分配的所有描述符集会自动回收重新变为可用
void createDescriptorPool() {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	//
	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;

		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}
}

void createGraphicsPipeline() {
	auto vertShaderCode = readFile(getExeDirectory() + "\\shader\\vertex.vert.spv");
	auto fragShaderCode = readFile(getExeDirectory() + "\\shader\\fragment.frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	//顶点着色器
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";			//指定主入口

	//片元着色器
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//顶点数据赋值
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = 5 * sizeof(float);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = 0;

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = 2 * sizeof(float);

	//VkVertexInputAttributeDescription attributeDescription = {};
	//attributeDescription.binding = 0;
	//attributeDescription.location = 0;
	//attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;	//vec2
	//attributeDescription.offset = 0;

	//1.管线的顶点输入
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	//2.图元装配
	//VK_PRIMITIVE_TOPOLOGY_POINT_LIST: 点
	//VK_PRIMITIVE_TOPOLOGY_LINE_LIST : 线（每两个顶点构成一个线段图元，每一对顶点相互独立）
	//VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : 线（从第二个顶点开始，每个顶点都与前一个顶点共同构成一条线。例如，如果你有四个顶点v1、v2、v3和v4，那么这种拓扑类型将创建三条线，分别是v1到v2，v2到v3，v3到v4）
	//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : 三角形（每三个顶点构成一个三角图元，三个顶点相互独立。例如，如果你有六个顶点v1、v2、v3、v4、v5和v6，那么这种拓扑类型将创建两个三角形，一个由v1、v2、v3组成，另一个由v4、v5、v6组成）
	//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : 三角形（从第三个顶点开始，每个新顶点与它之前的两个顶点共同构成一个新的三角形。例如，如果你有四个顶点v1、v2、v3和v4，那么这种拓扑类型将创建两个三角形，分别是（v1、v2、v3）和（v2、v3、v4））
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;	//是否开启图元重启。如果设置为true，如果遇到0xFFFF(16位)或者0xFFFFFFFF（32位）这些索引值的话，怎会结束当前图元，然后再进行新的图元创建。比如拓扑使用VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP，索引位{0, 1, 2, 0xFFFFFFFF, 3, 4, 5}，则会那么将生成两个三角形。第一个是由索引{0, 1, 2}定义，第二个是由索引{3, 4, 5}定义

	//3.裁剪和视口
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	//4.光栅化
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;			//VK_FALSE：有超出深度范围的部分将会被剪裁。VK_TRUE：超出摄像机近平面和远平面的片段将被不会被裁剪。具体来说，即使片段超出了深度视锥体范围，它仍然会被渲染，只不过超过的部分的深度值会被设置为近平面或远平面的值。这个设置在阴影贴图(shadow mapping)中很有用，因为它允许超出光源视锥体的物体能够投射阴影
	rasterizer.rasterizerDiscardEnable = VK_FALSE;	//如果为true的话，就不执行光栅化以及后面操作，通常可用于gpu计算，且不需要呈现到屏幕上。
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;	//填充
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;	//面剔除
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;	//正面，顺时针
	rasterizer.depthBiasEnable = VK_FALSE;	//不开启深度偏移设定

	//5.多重采样
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;				//关闭
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//VkPipelineDepthStencilStateCreateInfo深度和模板测试

	//6.颜色混合
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;	//不开启颜色混合
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;		//关闭逻辑操作。逻辑操作和混合操作会互斥：当启用逻辑操作时，混合操作被禁用
	colorBlending.logicOp = VK_LOGIC_OP_COPY; //Optional 逐位运算（AND、OR、XOR等） VK_LOGIC_OP_COPY：源的值	
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional		//设置混合常量，当混合因子为：VK_BLEND_FACTOR_CONSTANT_COLOR、VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR、VK_BLEND_FACTOR_CONSTANT_ALPHA和VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA的时候起效
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	//7.可动态更改的状态,并且不需要重新创建或重新编译整个管线
	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,	//视口大小和位置
	VK_DYNAMIC_STATE_SCISSOR,	//剪裁矩形区域
	//VK_DYNAMIC_STATE_LINE_WIDTH,	//线宽
	//VK_DYNAMIC_STATE_DEPTH_BIAS,	//深度偏置设定
	//VK_DYNAMIC_STATE_BLEND_CONSTANTS	//混合常量设定
	};

	//这个结构体会告诉驱动程序哪些状态不包含在管线状态中，可以在每个绘制调用中通过命令来单独改变
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//8.管线布局 目的是为shader传递参数
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());

	//为交换链的每一个图像视图对象创建对应的帧缓冲
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//VK_COMMAND_POOL_CREATE_TRANSIENT_BIT:使用它分配的指令缓冲对象被频繁用来记录新的指令(使用这一标记可能会改变帧缓冲对象的内存分配策略)。
	//VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 指令缓冲对象之间相互独立，不会被一起重置。不使用这一标记，指令缓冲对象会被放在一起重置
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//可以同时使用（或操作|），或者不设置
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void createCommandBuffer() {
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	//VK_COMMAND_BUFFER_LEVEL_PRIMARY：可以被提交到队列进行执行，但不能被其它指令缓冲对象调用，可以调用二级命令
	//VK_COMMAND_BUFFER_LEVEL_SECONDARY：不能直接被提交到队列进行执行，但可以被主要指令缓冲对象调用执行，这个就是二级命令
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();;

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: 指令缓冲在执行一次后，就被用来记录新的指令
	//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT : 这是一个只在一个渲染流程内使用的辅助指令缓冲
	//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT :在指令缓冲等待执行时，仍然可以提交这一指令缓冲
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];	//指定使用的渲染流程对

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;	//指定用于渲染的区域

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;		//于指定使用VK_ATTACHMENT_LOAD_OP_CLEAR标记后，使用的清除值
	
	//VK_SUBPASS_CONTENTS_INLINE: 所有要执行的指令都在主要指令缓冲中，没有辅助指令缓冲需要执行。
	//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : 有来自辅助指令缓冲的指令需要执行
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

		// vertexCount：尽管这里我们没有使用顶点缓冲，但仍然需要指定三个顶点用于三角形的绘制。 
		// instanceCount：用于实例渲染，为1时表示不进行实例渲染。 
		// firstVertex：用于定义着色器变量gl_VertexIndex的值。 
		// firstInstance：用于定义着色器变量gl_InstanceIndex的值
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

//简单来说fence是cpu和gpu的同步，semaphore是GPU内部或GPU之间的同步
void createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	//用于创建信号量对象。信号量是同步GPU命令执行的一种方式，特别是在多个队列和多个操作之间
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fence对象是用于跟踪具有特定事件的GPU操作何时完成的同步对象
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//默认已信号
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

}

void cleanupSwapChain() {
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void recreateSwapChain() {	
	//最小化窗口
	int width = 0, height = 0;
	glfwGetFramebufferSize(pWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(pWindow, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createFramebuffers();
}

void updateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void drawFrame() {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);	//等待缓冲区指令完成

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);	//从交换链中获取下面渲染所需要的图像，获取成功会发送imageAvailableSemaphore的信号量

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	 updateUniformBuffer(currentFrame);
	
	vkResetFences(device, 1, &inFlightFences[currentFrame]);	//此函数用于重置一个或多个Fence对象的状态为未信号

	vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;	//等待从交换链获取到图像的信号
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;	//完整后发出渲染结束的信号

	//提交指令缓冲
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {	//结束后inFlightFence会变成已信号
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;	//等待渲染结束的信号

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	//返回渲染后的图像到交换链进行呈现操作
	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}


	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//Vulkan实例、物理设备和逻辑设备：
//
//1.实例（VkInstance）：这是 Vulkan 程序的初始化和管理调用的起点。你可以把它看作是与Vulkan库本身的接口。它在程序中通常只存在一次，并在Vulkan应用程序的生命周期内持续存在。实例管理物理设备的发现。
//
//2.物理设备（VkPhysicalDevice）：物理设备代表一个实际的GPU（或其他设备如集成显卡），它被 Vulkan 实例发现并管理。物理设备的特性（类似是否支持特定的图形和计算操作以及设备相关的内存限制等）对应于具体础硬件的特性。
//
//3.逻辑设备（VkDevice）：一个逻辑设备代表应用程序和一个物理设备的接口。举例来说，你可以把逻辑设备视为应用程序"打开"的物理设备的实例。一旦打开，应用程序可以通过逻辑设备向物理设备提交命令。
//
//关系简图如下：
//VkInstance(Vulkan 库接口)
//|
//|----VkPhysicalDevice(GPU)
//|        |
//|        |-- - VkDevice(应用程序和 GPU 的连接)
//| ----VkPhysicalDevice(另一个 GPU)
//         |
//         |-- - VkDevice(应用程序和 另一个 GPU 的连接)
void initVulkan() {
	createInstance();//创建Vulkan实例
	setupDebugMessenger(); //设置Vulkan调试信息回调的函数
	createSurface();	//创建窗口surface
	pickPhysicalDevice();//选择物理设备(GPU)
	createLogicalDevice();//创建逻辑设备（通过创建一个逻辑设备，应用程序可以与物理设备进行交互，例如提交渲染和计算任务）
	createSwapChain();	//创建交换链，将图像呈现到屏幕
	createImageViews();	//创建图像视图
	createRenderPass();	//创建渲染pass
	createDescriptorSetLayout();
	createGraphicsPipeline(); //创建渲染管线
	createFramebuffers(); //创建帧缓冲区
	createCommandPool();	//创建命令池
	createTextureImage();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffer();	//创建命令缓冲区
	createSyncObjects();	//创建同步对象
}

void cleanupVulkan()
{
	cleanupSwapChain();

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

int main()
{
	glfwInit();

	//GLFW_CLIENT_API的默认设置是GLFW_OPENGL_API，这种情况下，GLFW会在创建窗口时创建OpenGL的上下文，这对于Vulkan而言是多余的，所以向GLFW说明不需要OpenGL的API。
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		//禁止窗口缩放
	pWindow = glfwCreateWindow(1280, 720, windowTitle, nullptr, nullptr);
	glfwSetFramebufferSizeCallback(pWindow, framebufferResizeCallback);

	initVulkan();

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
		TitleFps();
		drawFrame();
	}

	vkDeviceWaitIdle(device);

	cleanupVulkan();

	glfwDestroyWindow(pWindow);

	glfwTerminate();

	return 0;
}