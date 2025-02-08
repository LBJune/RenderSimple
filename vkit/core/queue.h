#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vkit
{
	class Device;

	class Queue
	{
	public:
		Queue(Device& device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index);

		Queue(const Queue&) = default;

		Queue(Queue&& other);

		Queue& operator=(const Queue&) = delete;

		Queue& operator=(Queue&&) = delete;

		~Queue();

		const Device& get_device() const;

		VkQueue get_handle() const;

		uint32_t get_family_index() const;

		uint32_t get_index() const;

		const VkQueueFamilyProperties& get_properties() const;

		VkBool32 support_present() const;

		VkResult submit(const std::vector<VkSubmitInfo>& submit_infos, VkFence fence) const;

		//VkResult submit(const CommandBuffer& command_buffer, VkFence fence) const;

		VkResult present(const VkPresentInfoKHR& present_infos) const;

		VkResult wait_idle() const;

	private:
		Device& device;

		VkQueue handle{ VK_NULL_HANDLE };

		uint32_t family_index{ 0 };

		uint32_t index{ 0 };

		VkBool32 can_present{ VK_FALSE };

		VkQueueFamilyProperties properties{};
	};
}        // namespace vkit
