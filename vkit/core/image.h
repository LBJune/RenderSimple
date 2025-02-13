#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_set>

namespace vkit
{
	class ImageView;
	class Device;
	class Image
	{
	public:
		Image(vkit::Device& device,
			VkImage               handle,
			const VkExtent3D& extent,
			VkFormat              format,
			VkImageUsageFlags     image_usage,
			VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);

		Image(
			vkit::Device& device,
			const VkExtent3D& extent,
			VkFormat              format,
			VkImageUsageFlags     image_usage,
			VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT,
			uint32_t              mip_levels = 1,
			uint32_t              array_layers = 1,
			VkImageTiling         tiling = VK_IMAGE_TILING_OPTIMAL,
			VkImageCreateFlags    flags = 0,
			uint32_t              num_queue_families = 0,
			const uint32_t* queue_families = nullptr,
			const void* pNext = nullptr,
			VkImageLayout         initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

		Image(const Image&) = delete;

		~Image();

		Image& operator=(const Image&) = delete;

		Image& operator=(Image&&) = delete;

		Device& get_device();

		VkImage get_handle() const;

		VkImageType get_type() const;

		const VkExtent3D& get_extent() const;

		VkFormat get_format() const;

		VkSampleCountFlagBits get_sample_count() const;

		VkImageUsageFlags get_usage() const;

		VkImageTiling get_tiling() const;

		const VkImageSubresource& get_subresource() const;

		uint32_t get_array_layer_count() const;

		std::unordered_set<ImageView*>& get_views();
	private:
		Device& device;

		VkImage handle{ VK_NULL_HANDLE };

		bool is_new_alloc = false;

		/// Image views referring to this image
		VkImageCreateInfo               create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		VkImageSubresource              subresource{};
		std::unordered_set<ImageView*> views;
	};
}        // namespace vkit
