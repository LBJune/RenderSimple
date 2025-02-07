#include "instance.h"

#include <algorithm>
#include <functional>
#include <utils/log.h>
#include <utils/error.h>

#ifdef _DEBUG
#	define USE_VALIDATION_LAYERS
#endif

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

	Instance::Instance(const std::string&			application_name,
		const std::vector<const char*>&				required_extensions,
		const std::vector<const char*>&				required_validation_layers,
		uint32_t                                    api_version)
	{
		uint32_t instance_extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
		std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data());

#ifdef USE_VALIDATION_LAYERS
		enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, available_instance_extensions, enabled_extensions);
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

		createInfo.enabledExtensionCount = enabled_extensions.size();
		createInfo.ppEnabledExtensionNames = enabled_extensions.data();

		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	Instance::Instance(VkInstance instance) :
		handle{ instance }
	{
		
	}

	Instance::~Instance()
	{
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
}        // namespace vkit
