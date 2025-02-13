#include "physical_device.h"
#include <utils/log.h>
#include <utils/error.h>
#include "instance.h"

namespace vkit
{
	PhysicalDevice::PhysicalDevice(Instance& instance, VkPhysicalDevice physical_device):
		instance{ instance },
		handle{ physical_device }
	{
		vkGetPhysicalDeviceFeatures(physical_device, &features);
		vkGetPhysicalDeviceProperties(physical_device, &properties);
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

		LOGI("Found GPU: %s", properties.deviceName);

		uint32_t queue_family_properties_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, nullptr);
		queue_family_properties = std::vector<VkQueueFamilyProperties>(queue_family_properties_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, queue_family_properties.data());

		uint32_t device_extension_count;
		VK_CHECK(vkEnumerateDeviceExtensionProperties(get_handle(), nullptr, &device_extension_count, nullptr));
		device_extensions = std::vector<VkExtensionProperties>(device_extension_count);
		VK_CHECK(vkEnumerateDeviceExtensionProperties(get_handle(), nullptr, &device_extension_count, device_extensions.data()));

		// Display supported extensions
		if (device_extensions.size() > 0)
		{
			LOGD("Device supports the following extensions:");
			for (auto& extension : device_extensions)
			{
				LOGD("  \t%s", extension.extensionName);
			}
		}
	}

	PhysicalDevice::~PhysicalDevice()
	{

	}

	Instance& PhysicalDevice::get_instance() const
	{
		return instance;
	}

	VkPhysicalDevice PhysicalDevice::get_handle() const
	{
		return handle;
	}

	const VkPhysicalDeviceFeatures& PhysicalDevice::get_features() const
	{
		return features;
	}

	const VkPhysicalDeviceProperties& PhysicalDevice::get_properties() const
	{
		return properties;
	}

	const VkPhysicalDeviceMemoryProperties& PhysicalDevice::get_memory_properties() const
	{
		return memory_properties;
	}

	const std::vector<VkQueueFamilyProperties>& PhysicalDevice::get_queue_family_properties() const
	{
		return queue_family_properties;
	}

	VkBool32 PhysicalDevice::is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_index) const
	{
		VkBool32 present_supported{ VK_FALSE };

		if (surface != VK_NULL_HANDLE)
		{
			VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(handle, queue_family_index, surface, &present_supported));
		}

		return present_supported;
	}

	bool PhysicalDevice::is_extension_supported(const char * requested_extension) const
	{
		return std::find_if(device_extensions.begin(), device_extensions.end(),
			[requested_extension](auto& device_extension) {
				return std::strcmp(device_extension.extensionName, requested_extension) == 0;
			}) != device_extensions.end();
	}
}        // namespace vkit
