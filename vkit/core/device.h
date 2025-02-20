#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace vkit
{
class PhysicalDevice;
class Queue;
class Device
{
  public:
	/**
	 * @brief Device constructor
	 * @param gpu A valid Vulkan physical device and the requested gpu features
	 */
	Device(PhysicalDevice&			gpu,
		   VkSurfaceKHR				surface,
		   std::vector<const char*> requested_extensions = {});

	Device(const Device &) = delete;

	Device(Device &&) = delete;

	~Device();

	Device &operator=(const Device &) = delete;

	Device &operator=(Device &&) = delete;

	VkDevice get_handle() const;

	const PhysicalDevice &get_gpu() const;

	uint32_t get_queue_family_index(VkQueueFlagBits queue_flag);

	bool is_extension_supported(const char* extension) const;

	bool is_enabled(const char* extension) const;
  private:
	const PhysicalDevice &gpu;

	VkDevice handle { VK_NULL_HANDLE };

	std::vector<std::vector<Queue>> queues;

	std::vector<const char*> enabled_extensions{};
};
}        // namespace vkit
