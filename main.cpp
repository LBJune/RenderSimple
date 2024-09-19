
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <optional>
#include <set>

GLFWwindow* pWindow = NULL;
//窗口标题
const char* windowTitle = "Vulkan Window";

VkInstance instance = VK_NULL_HANDLE;

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkDevice device;
VkQueue graphicsQueue;
VkQueue presentQueue;

VkSurfaceKHR surface;

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

//选择符合要求的设备
bool isDeviceSuitable(VkPhysicalDevice device) {
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);//基础的设备属性，比如名称，类型和支持的Vulkan版本

	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);//查询功能特性，比如纹理压缩，64位浮点和多视口渲染(常用于VR)等特性的支持

	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.isComplete();

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

	createInfo.enabledExtensionCount = 0;

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

}

int main()
{
	glfwInit();

	//GLFW_CLIENT_API的默认设置是GLFW_OPENGL_API，这种情况下，GLFW会在创建窗口时创建OpenGL的上下文，这对于Vulkan而言是多余的，所以向GLFW说明不需要OpenGL的API。
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		//禁止窗口缩放
	pWindow = glfwCreateWindow(1280, 720, windowTitle, nullptr, nullptr);

	initVulkan();

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);


	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
		TitleFps();
	}

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroyDevice(device, nullptr);

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(pWindow);

	glfwTerminate();

	return 0;
}