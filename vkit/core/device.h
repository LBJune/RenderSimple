#pragma once

#include <vulkan/vulkan_core.h>

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
	Device(PhysicalDevice& gpu);

	Device(const Device &) = delete;

	Device(Device &&) = delete;

	~Device();

	Device &operator=(const Device &) = delete;

	Device &operator=(Device &&) = delete;

	VkDevice get_handle() const;

	const PhysicalDevice &get_gpu() const;

  private:
	const PhysicalDevice &gpu;

	VkDevice handle { VK_NULL_HANDLE };

	Queue* queue {nullptr};

};
}        // namespace vkit
