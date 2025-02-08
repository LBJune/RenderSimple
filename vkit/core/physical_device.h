#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vkit
{
class Instance;
/**
 * @brief A wrapper class for VkPhysicalDevice
 *
 * This class is responsible for handling gpu features, properties, and queue families for the device creation.
 */
class PhysicalDevice
{
  public:
	PhysicalDevice(Instance &instance, VkPhysicalDevice physical_device);

	PhysicalDevice(const PhysicalDevice &) = delete;

	PhysicalDevice(PhysicalDevice &&) = delete;

	PhysicalDevice &operator=(const PhysicalDevice &) = delete;

	PhysicalDevice &operator=(PhysicalDevice &&) = delete;

	~PhysicalDevice();

	Instance& get_instance() const;

	VkPhysicalDevice get_handle() const;

	const VkPhysicalDeviceFeatures& get_features() const;

	const VkPhysicalDeviceProperties& get_properties() const;

	const VkPhysicalDeviceMemoryProperties& get_memory_properties() const;

	const std::vector<VkQueueFamilyProperties>& get_queue_family_properties() const;

	VkBool32 is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_index) const;

	/**
	 * @brief Sets whether or not the first graphics queue should have higher priority than other queues.
	 * Very specific feature which is used by async compute samples.
	 * @param enable If true, present queue will have prio 1.0 and other queues have prio 0.5.
	 * Default state is false, where all queues have 0.5 priority.
	*/
	void set_high_priority_graphics_queue_enable(bool enable)
	{
		high_priority_graphics_queue = enable;
	}

	/**
	 * @brief Returns high priority graphics queue state.
	 * @return High priority state.
	 */
	bool has_high_priority_graphics_queue() const
	{
		return high_priority_graphics_queue;
	}

  private:
	// Handle to the Vulkan instance
	Instance& instance;

	// Handle to the Vulkan physical device
	VkPhysicalDevice handle{ VK_NULL_HANDLE };

	// The features that this GPU supports
	VkPhysicalDeviceFeatures features{};

	// The GPU properties
	VkPhysicalDeviceProperties properties;

	// The GPU memory properties
	VkPhysicalDeviceMemoryProperties memory_properties;

	// The GPU queue family properties
	std::vector<VkQueueFamilyProperties> queue_family_properties;

	bool high_priority_graphics_queue{};
};

}        // namespace vkit
