#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#ifdef _DEBUG
#	define USE_VALIDATION_LAYERS
#endif


namespace vkit
{
	class PhysicalDevice;
	/**
	 * @brief A wrapper class for VkInstance
	 */
	class Instance
	{
	public:
		Instance(const std::string&				application_name,
			const std::vector<const char*>&		required_extensions = {},
			const std::vector<const char*>&		required_validation_layers = {},
			uint32_t                            api_version = VK_API_VERSION_1_0);

		/**
		 * @brief Queries the GPUs of a VkInstance that is already created
		 * @param instance A valid VkInstance
		 */
		Instance(VkInstance instance);

		Instance(const Instance&) = delete;

		Instance(Instance&&) = delete;

		~Instance();

		Instance& operator=(const Instance&) = delete;

		Instance& operator=(Instance&&) = delete;

		VkInstance get_handle() const;

		const std::vector<const char*>& get_extensions();

		PhysicalDevice& pick_physical_device();

		PhysicalDevice& get_suitable_gpu(VkSurfaceKHR surface);

		/**
		 * @brief Can be set from the GPU selection plugin to explicitly select a GPU instead
		 */
		std::optional<uint32_t> selected_gpu_index;

	private:
		/**
		 * @brief The Vulkan instance
		 */
		VkInstance handle{ VK_NULL_HANDLE };

		/**
		 * @brief The enabled extensions
		 */
		std::vector<const char*> enabled_extensions;

		void setup_debug_essenger();

		/**
		 * @brief Queries the instance for the physical devices on the machine
		 */
		void query_gpus();

		/**
		 * @brief The physical devices found on the machine
		 */
		std::vector<std::unique_ptr<PhysicalDevice>> gpus;

#if defined(USE_VALIDATION_LAYERS)
		/**
		 * @brief Debug utils messenger callback for VK_EXT_Debug_Utils
		 */
		VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };
#endif
	};
}        // namespace vkit
