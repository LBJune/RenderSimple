#include "device.h"
#include "physical_device.h"
#include "instance.h"
#include "queue.h"

#include <utils/log.h>
#include <utils/error.h>

namespace vkit
{

	Device::Device(PhysicalDevice& physical_device) :
		gpu{ physical_device }
	{
		LOGI("Selected GPU: %s", gpu.get_properties().deviceName);

		uint32_t queueFamilyIndex = gpu.get_instance().selected_gpu_index.value();

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (vkCreateDevice(gpu.get_handle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		queue = new Queue(*this, queueFamilyIndex, gpu.get_queue_family_properties()[queueFamilyIndex], VK_FALSE, 0);
	
	}

	Device::~Device()
	{
		if (queue)
		{
			delete queue;
			queue = nullptr;
		}
	}


	VkDevice Device::get_handle() const
	{
		return handle;
	}

	const PhysicalDevice& Device::get_gpu() const
	{
		return gpu;
	}

}        // namespace vkit
