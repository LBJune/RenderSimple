#include "instance.h"
#include "physical_device.h"

#include <algorithm>
#include <functional>
#include <utils/log.h>
#include <utils/error.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	LOGD( "validation layer: %s", pCallbackData->pMessage);
	return VK_FALSE;
}

namespace vkit
{
	bool enable_extension(const char* required_ext_name,
		const std::vector<VkExtensionProperties>& available_exts,
		std::vector<const char*>& enabled_extensions)
	{
		for (auto& avail_ext_it : available_exts)
		{
			if (strcmp(avail_ext_it.extensionName, required_ext_name) == 0)
			{
				auto it = std::find_if(enabled_extensions.begin(), enabled_extensions.end(),
					[required_ext_name](const char* enabled_ext_name) {
						return strcmp(enabled_ext_name, required_ext_name) == 0;
					});
				if (it != enabled_extensions.end())
				{
					// Extension is already enabled
				}
				else
				{
					LOGI("Extension %s found, enabling it", required_ext_name);
					enabled_extensions.emplace_back(required_ext_name);
				}
				return true;
			}
		}

		LOGI("Extension %s not found", required_ext_name);
		return false;
	}

	bool checkValidationLayerSupport(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available) 
	{
        for (const char* layerName : required) {
            bool layerFound = false;

            for (const auto& layerProperties : available) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
				LOGE("Validation Layer %s not found", layerName);
                return false;
            }
        }

        return true;
    }

	std::vector<const char*> get_optimal_validation_layers(const std::vector<VkLayerProperties>& supported_instance_layers)
	{
		std::vector<std::vector<const char*>> validation_layer_priority_list =
		{
			// The preferred validation layer is "VK_LAYER_KHRONOS_validation"
			{"VK_LAYER_KHRONOS_validation"},

			// Otherwise we fallback to using the LunarG meta layer
			{"VK_LAYER_LUNARG_standard_validation"},

			// Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
			{
				"VK_LAYER_GOOGLE_threading",
				"VK_LAYER_LUNARG_parameter_validation",
				"VK_LAYER_LUNARG_object_tracker",
				"VK_LAYER_LUNARG_core_validation",
				"VK_LAYER_GOOGLE_unique_objects",
			},

			// Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
			{"VK_LAYER_LUNARG_core_validation"} };

		for (auto& validation_layers : validation_layer_priority_list)
		{
			if (checkValidationLayerSupport(validation_layers, supported_instance_layers))
			{
				return validation_layers;
			}

			LOGW("Couldn't enable validation layers (see log for error) - falling back");
		}

		// Else return nothing
		return {};
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
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

	Instance::Instance(const std::string&			application_name,
		const std::vector<const char*>&				required_extensions,
		const std::vector<const char*>&				required_validation_layers,
		uint32_t                                    api_version)
	{
		uint32_t instance_layer_count;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

		std::vector<VkLayerProperties> supported_validation_layers(instance_layer_count);
		VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_validation_layers.data()));

		if (!checkValidationLayerSupport(required_validation_layers, supported_validation_layers)) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		uint32_t instance_extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
		std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data());

		std::vector<const char*> requested_validation_layers(required_validation_layers);

#ifdef USE_VALIDATION_LAYERS
		enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, available_instance_extensions, enabled_extensions);
		std::vector<const char*> optimal_validation_layers = get_optimal_validation_layers(supported_validation_layers);
		requested_validation_layers.insert(requested_validation_layers.end(), optimal_validation_layers.begin(), optimal_validation_layers.end());
#endif

		for (auto extension : required_extensions)
		{
			enable_extension(extension, available_instance_extensions, enabled_extensions);
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = application_name.c_str();
		appInfo.applicationVersion = 0;
		appInfo.pEngineName = "VKit Engine";
		appInfo.engineVersion = 0;
		appInfo.apiVersion = api_version;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
		createInfo.ppEnabledExtensionNames = enabled_extensions.data();

		createInfo.enabledLayerCount = static_cast<uint32_t>(requested_validation_layers.size());
		createInfo.ppEnabledLayerNames = requested_validation_layers.data();

#ifdef USE_VALIDATION_LAYERS
		////Debugging instance creation and destruction
		//VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		//populateDebugMessengerCreateInfo(debugCreateInfo);
		//createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

		if (vkCreateInstance(&createInfo, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
#ifdef USE_VALIDATION_LAYERS
		setup_debug_essenger();
#endif
		query_gpus();
	}

	Instance::Instance(VkInstance instance) :
		handle{ instance }
	{
		if (handle != VK_NULL_HANDLE)
		{
			query_gpus();
		}
		else
		{
			throw std::runtime_error("Instance not valid");
		}
	}

	Instance::~Instance()
	{
		if (debug_utils_messenger != VK_NULL_HANDLE)
		{
			DestroyDebugUtilsMessengerEXT(handle, debug_utils_messenger, nullptr);
		}

		if (handle != VK_NULL_HANDLE)
		{
			vkDestroyInstance(handle, nullptr);
		}
	}

	VkInstance Instance::get_handle() const
	{
		return handle;
	}

	const std::vector<const char*>& Instance::get_extensions()
	{
		return enabled_extensions;
	}

	void Instance::setup_debug_essenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(handle, &createInfo, nullptr, &debug_utils_messenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void Instance::query_gpus()
	{
		// Querying valid physical devices on the machine
		uint32_t physical_device_count{ 0 };
		VK_CHECK(vkEnumeratePhysicalDevices(handle, &physical_device_count, nullptr));

		if (physical_device_count < 1)
		{
			throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
		}

		std::vector<VkPhysicalDevice> physical_devices;
		physical_devices.resize(physical_device_count);
		VK_CHECK(vkEnumeratePhysicalDevices(handle, &physical_device_count, physical_devices.data()));

		// Create gpus wrapper objects from the VkPhysicalDevice's
		for (auto& physical_device : physical_devices)
		{
			gpus.push_back(std::make_unique<PhysicalDevice>(*this, physical_device));
		}
	}

	PhysicalDevice& Instance::pick_physical_device()
	{
		assert(!gpus.empty() && "No physical devices were found on the system.");

		// Find a discrete GPU
		for (auto& gpu : gpus)
		{
			if (gpu->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				return *gpu;
			}
		}

		// Otherwise just pick the first one
		LOGW("Couldn't find a discrete physical device, picking default GPU");
		return *gpus[0];

	}

	PhysicalDevice& Instance::get_suitable_gpu(VkSurfaceKHR surface)
	{
		assert(!gpus.empty() && "No physical devices were found on the system.");

		// Find a discrete GPU
		for (auto& gpu : gpus)
		{
			if (gpu->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				// See if it work with the surface
				size_t queue_count = gpu->get_queue_family_properties().size();
				for (uint32_t queue_idx = 0; static_cast<size_t>(queue_idx) < queue_count; queue_idx++)
				{
					if (gpu->is_present_supported(surface, queue_idx))
					{
						selected_gpu_index = queue_idx;
						return *gpu;
					}
				}
			}
		}

		// Otherwise just pick the first one
		LOGW("Couldn't find a discrete physical device, picking default GPU");
		return *gpus[0];
	}
}        // namespace vkit
