#include "device.h"
#include "physical_device.h"
#include "instance.h"
#include "queue.h"

#include <utils/log.h>
#include <utils/error.h>

namespace vkit
{

	Device::Device(PhysicalDevice& physical_device,
		VkSurfaceKHR               surface) :
		gpu{ physical_device }
	{
		LOGI("Selected GPU: %s", gpu.get_properties().deviceName);

		// Prepare the device queues
		uint32_t                             queue_family_properties_count = static_cast<uint32_t>(gpu.get_queue_family_properties().size());
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_family_properties_count, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
		std::vector<std::vector<float>>      queue_priorities(queue_family_properties_count);

		for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index)
		{
			const VkQueueFamilyProperties& queue_family_property = gpu.get_queue_family_properties()[queue_family_index];

			if (gpu.has_high_priority_graphics_queue())
			{
				uint32_t graphics_queue_family = get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
				if (graphics_queue_family == queue_family_index)
				{
					queue_priorities[queue_family_index].reserve(queue_family_property.queueCount);
					queue_priorities[queue_family_index].push_back(1.0f);
					for (uint32_t i = 1; i < queue_family_property.queueCount; i++)
					{
						queue_priorities[queue_family_index].push_back(0.5f);
					}
				}
				else
				{
					queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
				}
			}
			else
			{
				queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
			}

			VkDeviceQueueCreateInfo& queue_create_info = queue_create_infos[queue_family_index];

			queue_create_info.queueFamilyIndex = queue_family_index;
			queue_create_info.queueCount = queue_family_property.queueCount;
			queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = queue_create_infos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (vkCreateDevice(gpu.get_handle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		queues.resize(queue_family_properties_count);

		for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index)
		{
			const VkQueueFamilyProperties& queue_family_property = gpu.get_queue_family_properties()[queue_family_index];

			VkBool32 present_supported = gpu.is_present_supported(surface, queue_family_index);

			for (uint32_t queue_index = 0U; queue_index < queue_family_property.queueCount; ++queue_index)
			{
				queues[queue_family_index].emplace_back(*this, queue_family_index, queue_family_property, present_supported, queue_index);
			}
		}

	
	}

	Device::~Device()
	{
		if (handle != VK_NULL_HANDLE)
		{
			vkDestroyDevice(handle, nullptr);
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

	uint32_t Device::get_queue_family_index(VkQueueFlagBits queue_flag)
	{
		const auto& queue_family_properties = gpu.get_queue_family_properties();

		// Dedicated queue for compute
		// Try to find a queue family index that supports compute but not graphics
		if (queue_flag & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
			{
				if ((queue_family_properties[i].queueFlags & queue_flag) && !(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
				{
					return i;
					break;
				}
			}
		}

		// Dedicated queue for transfer
		// Try to find a queue family index that supports transfer but not graphics and compute
		if (queue_flag & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
			{
				if ((queue_family_properties[i].queueFlags & queue_flag) && !(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
				{
					return i;
					break;
				}
			}
		}

		// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
		for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
		{
			if (queue_family_properties[i].queueFlags & queue_flag)
			{
				return i;
				break;
			}
		}

		throw std::runtime_error("Could not find a matching queue family index");
	}

}        // namespace vkit
