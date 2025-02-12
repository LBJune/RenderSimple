
#include "swapchain.h"
#include "device.h"
#include "physical_device.h"

#include <utils/log.h>
#include <utils/error.h>
#include <utils/strings.h>
#include <algorithm>

namespace vkit
{
	inline uint32_t choose_image_count(
		uint32_t request_image_count,
		uint32_t min_image_count,
		uint32_t max_image_count)
	{
		if (max_image_count != 0)
		{
			request_image_count = std::min(request_image_count, max_image_count);
		}

		request_image_count = std::max(request_image_count, min_image_count);

		return request_image_count;
	}

	inline uint32_t choose_image_array_layers(
		uint32_t request_image_array_layers,
		uint32_t max_image_array_layers)
	{
		request_image_array_layers = std::min(request_image_array_layers, max_image_array_layers);
		request_image_array_layers = std::max(request_image_array_layers, 1u);

		return request_image_array_layers;
	}

	inline VkExtent2D choose_extent(
		VkExtent2D        request_extent,
		const VkExtent2D& min_image_extent,
		const VkExtent2D& max_image_extent,
		const VkExtent2D& current_extent)
	{
		if (current_extent.width == 0xFFFFFFFF)
		{
			return request_extent;
		}

		if (request_extent.width < 1 || request_extent.height < 1)
		{
			LOGW("(Swapchain) Image extent (%d, %d) not supported. Selecting (%d, %d).", request_extent.width, request_extent.height, current_extent.width, current_extent.height);
			return current_extent;
		}

		request_extent.width = std::clamp(request_extent.width, min_image_extent.width, max_image_extent.width);
		request_extent.height = std::clamp(request_extent.height, min_image_extent.height, max_image_extent.height);

		return request_extent;
	}

	inline VkPresentModeKHR choose_present_mode(
		const std::vector<VkPresentModeKHR>& available_present_modes,
		const std::vector<VkPresentModeKHR>& present_mode_priority_list)
	{
		auto present_mode_it = available_present_modes.begin();

		for (auto& present_mode : present_mode_priority_list)
		{
			present_mode_it = std::find(available_present_modes.begin(), available_present_modes.end(), present_mode);
			if (present_mode_it != available_present_modes.end())
			{
				break;
			}
		}

		LOGI("(Swapchain) Present mode selected: %s", to_string(*present_mode_it).c_str());
		return *present_mode_it;
	}


	inline VkSurfaceFormatKHR choose_surface_format(
		const std::vector<VkSurfaceFormatKHR>& available_surface_formats,
		const std::vector<VkSurfaceFormatKHR>& surface_format_priority_list)
	{
		// Try to find the requested surface format in the supported surface formats
		auto surface_format_it = available_surface_formats.begin();

		// Try to request a format from the priority list
		for (auto& surface_format : surface_format_priority_list)
		{
			surface_format_it = std::find_if(
				available_surface_formats.begin(),
				available_surface_formats.end(),
				[&surface_format](const VkSurfaceFormatKHR& surface) {
					if (surface.format == surface_format.format &&
						surface.colorSpace == surface_format.colorSpace)
					{
						return true;
					}

					return false;
				});
			if (surface_format_it != available_surface_formats.end())
			{
				break;
			}
		}

		LOGI("(Swapchain) Surface format selected: %s", to_string(*surface_format_it).c_str());
		return *surface_format_it;
	}

	inline VkSurfaceTransformFlagBitsKHR choose_transform(
		VkSurfaceTransformFlagBitsKHR request_transform,
		VkSurfaceTransformFlagsKHR    supported_transform,
		VkSurfaceTransformFlagBitsKHR current_transform)
	{
		if (request_transform & supported_transform)
		{
			return request_transform;
		}

		LOGW("(Swapchain) Surface transform '%s' not supported. Selecting '%s'.", to_string(request_transform).c_str(), to_string(current_transform).c_str());

		return current_transform;
	}

	inline VkCompositeAlphaFlagBitsKHR choose_composite_alpha(VkCompositeAlphaFlagBitsKHR request_composite_alpha, VkCompositeAlphaFlagsKHR supported_composite_alpha)
	{
		if (request_composite_alpha & supported_composite_alpha)
		{
			return request_composite_alpha;
		}

		static const std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR };

		for (VkCompositeAlphaFlagBitsKHR composite_alpha : composite_alpha_flags)
		{
			if (composite_alpha & supported_composite_alpha)
			{
				LOGW("(Swapchain) Composite alpha '%s' not supported. Selecting '%s.", to_string(request_composite_alpha).c_str(), to_string(composite_alpha).c_str());
				return composite_alpha;
			}
		}

		throw std::runtime_error("No compatible composite alpha found.");
	}

	inline bool validate_format_feature(VkImageUsageFlagBits image_usage, VkFormatFeatureFlags supported_features)
	{
		switch (image_usage)
		{
		case VK_IMAGE_USAGE_STORAGE_BIT:
			return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supported_features;
		default:
			return true;
		}
	}

	inline std::set<VkImageUsageFlagBits> choose_image_usage(const std::set<VkImageUsageFlagBits>& requested_image_usage_flags, VkImageUsageFlags supported_image_usage, VkFormatFeatureFlags supported_features)
	{
		std::set<VkImageUsageFlagBits> validated_image_usage_flags;
		for (auto flag : requested_image_usage_flags)
		{
			if ((flag & supported_image_usage) && validate_format_feature(flag, supported_features))
			{
				validated_image_usage_flags.insert(flag);
			}
			else
			{
				LOGW("(Swapchain) Image usage (%s) requested but not supported.", to_string(flag).c_str());
			}
		}

		if (validated_image_usage_flags.empty())
		{
			// Pick the first format from list of defaults, if supported
			static const std::vector<VkImageUsageFlagBits> image_usage_flags = {
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT };

			for (VkImageUsageFlagBits image_usage : image_usage_flags)
			{
				if ((image_usage & supported_image_usage) && validate_format_feature(image_usage, supported_features))
				{
					validated_image_usage_flags.insert(image_usage);
					break;
				}
			}
		}

		if (!validated_image_usage_flags.empty())
		{
			// Log image usage flags used
			std::string usage_list;
			for (VkImageUsageFlagBits image_usage : validated_image_usage_flags)
			{
				usage_list += to_string(image_usage) + " ";
			}
			LOGI("(Swapchain) Image usage flags: %s", usage_list.c_str());
		}
		else
		{
			throw std::runtime_error("No compatible image usage found.");
		}

		return validated_image_usage_flags;
	}

	inline VkImageUsageFlags composite_image_flags(std::set<VkImageUsageFlagBits>& image_usage_flags)
	{
		VkImageUsageFlags image_usage{};
		for (auto flag : image_usage_flags)
		{
			image_usage |= flag;
		}
		return image_usage;
	}

	Swapchain::Swapchain(Device& device,
		VkSurfaceKHR                              surface,
		std::vector<VkPresentModeKHR> const& present_mode_priority_list,
		const std::vector<VkSurfaceFormatKHR>& surface_format_priority_list,
		const VkExtent2D& extent,
		const uint32_t                            image_count,
		const VkSurfaceTransformFlagBitsKHR       transform,
		const std::set<VkImageUsageFlagBits>& image_usage_flags,
		const VkImageCompressionFlagsEXT          requested_compression,
		const VkImageCompressionFixedRateFlagsEXT requested_compression_fixed_rate) :
		Swapchain{ *this, device, surface, present_mode_priority_list, surface_format_priority_list, extent, image_count, transform, image_usage_flags }
	{
	}

	Swapchain::Swapchain(Swapchain& old_swapchain,
		Device& device,
		VkSurfaceKHR                              surface,
		std::vector<VkPresentModeKHR> const& present_mode_priority_list,
		const std::vector<VkSurfaceFormatKHR>& surface_format_priority_list,
		const VkExtent2D& extent,
		const uint32_t                            image_count,
		const VkSurfaceTransformFlagBitsKHR       transform,
		const std::set<VkImageUsageFlagBits>& image_usage_flags,
		const VkImageCompressionFlagsEXT          requested_compression,
		const VkImageCompressionFixedRateFlagsEXT requested_compression_fixed_rate) 
		:
		device{ device },
		surface{ surface }
	{

		VkSurfaceCapabilitiesKHR surface_capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device.get_gpu().get_handle(), surface, &surface_capabilities);		//查询给定sueface在物理设备上的基本能力，如尺寸、图像数量、图像数组大小

		uint32_t surface_format_count{ 0U };
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.get_gpu().get_handle(), surface, &surface_format_count, nullptr));

		std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.get_gpu().get_handle(), surface, &surface_format_count, surface_formats.data()));	 //查询给定sueface支持的格式

		LOGI("Surface supports the following surface formats:");
		for (auto& surface_format : surface_formats)
		{
			LOGI("  \t%s", to_string(surface_format).c_str());
		}

		uint32_t present_mode_count{ 0U };
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.get_gpu().get_handle(), surface, &present_mode_count, nullptr));

		std::vector<VkPresentModeKHR> present_modes(present_mode_count);
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.get_gpu().get_handle(), surface, &present_mode_count, present_modes.data()));		//查询给定的物理设备到surface的有效呈现模式

		LOGI("Surface supports the following present modes:");
		for (auto& pm : present_modes)
		{
			LOGI("  \t%s", to_string(pm).c_str());
		}
		
		// Choose best properties based on surface capabilities
		properties.old_swapchain = old_swapchain.get_handle();
		properties.image_count = choose_image_count(image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);
		properties.extent = choose_extent(extent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent, surface_capabilities.currentExtent);
		properties.surface_format = choose_surface_format(surface_formats, surface_format_priority_list);
		properties.array_layers = choose_image_array_layers(1U, surface_capabilities.maxImageArrayLayers);

		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(this->device.get_gpu().get_handle(), properties.surface_format.format, &format_properties);
		this->image_usage_flags = choose_image_usage(image_usage_flags, surface_capabilities.supportedUsageFlags, format_properties.optimalTilingFeatures);

		properties.image_usage = composite_image_flags(this->image_usage_flags);
		properties.pre_transform = choose_transform(transform, surface_capabilities.supportedTransforms, surface_capabilities.currentTransform);
		properties.composite_alpha = choose_composite_alpha(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, surface_capabilities.supportedCompositeAlpha);
		properties.present_mode = choose_present_mode(present_modes, present_mode_priority_list);

		VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		create_info.minImageCount = properties.image_count;
		create_info.imageExtent = properties.extent;
		create_info.presentMode = properties.present_mode;
		create_info.imageFormat = properties.surface_format.format;
		create_info.imageColorSpace = properties.surface_format.colorSpace;
		create_info.imageArrayLayers = properties.array_layers;
		create_info.imageUsage = properties.image_usage;
		create_info.preTransform = properties.pre_transform;
		create_info.compositeAlpha = properties.composite_alpha;
		create_info.oldSwapchain = properties.old_swapchain;
		create_info.surface = surface;

		if (vkCreateSwapchainKHR(device.get_handle(), &create_info, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}


		uint32_t image_available{ 0u };
		VK_CHECK(vkGetSwapchainImagesKHR(device.get_handle(), handle, &image_available, nullptr));

		images.resize(image_available);

		VK_CHECK(vkGetSwapchainImagesKHR(device.get_handle(), handle, &image_available, images.data()));

	}

	Swapchain::~Swapchain()
	{
		if (handle != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device.get_handle(), handle, nullptr);
		}
	}

	bool Swapchain::is_valid() const
	{
		return handle != VK_NULL_HANDLE;
	}

	Device& Swapchain::get_device()
	{
		return device;
	}

	VkSwapchainKHR Swapchain::get_handle() const
	{
		return handle;
	}

	VkResult Swapchain::acquire_next_image(uint32_t& image_index, VkSemaphore image_acquired_semaphore, VkFence fence) const
	{
		return vkAcquireNextImageKHR(device.get_handle(), handle, std::numeric_limits<uint64_t>::max(), image_acquired_semaphore, fence, &image_index);
	}

	const VkExtent2D& Swapchain::get_extent() const
	{
		return properties.extent;
	}

	VkFormat Swapchain::get_format() const
	{
		return properties.surface_format.format;
	}

	VkSurfaceFormatKHR Swapchain::get_surface_format() const
	{
		return properties.surface_format;
	}

	const std::vector<VkImage>& Swapchain::get_images() const
	{
		return images;
	}

	VkSurfaceTransformFlagBitsKHR Swapchain::get_transform() const
	{
		return properties.pre_transform;
	}

	VkSurfaceKHR Swapchain::get_surface() const
	{
		return surface;
	}

	VkImageUsageFlags Swapchain::get_usage() const
	{
		return properties.image_usage;
	}

	VkPresentModeKHR Swapchain::get_present_mode() const
	{
		return properties.present_mode;
	}

}        // namespace vkit
