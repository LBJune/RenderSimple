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
};

}        // namespace vkit
