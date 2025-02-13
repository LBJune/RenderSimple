#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vkit
{
	class Device;
	class Image;
	class ImageView
	{
	public:
		ImageView(Image& image, VkImageViewType view_type, VkFormat format = VK_FORMAT_UNDEFINED,
			uint32_t base_mip_level = 0, uint32_t base_array_layer = 0,
			uint32_t n_mip_levels = 0, uint32_t n_array_layers = 0);

		ImageView(ImageView&) = delete;

		~ImageView();

		ImageView& operator=(const ImageView&) = delete;

		ImageView& operator=(ImageView&&) = delete;

		VkImageView get_handle() const;

		const Image& get_image() const;

		/**
		 * @brief Update the image this view is referring to
		 *        Used on image move
		 */
		void set_image(Image& image);

		VkFormat get_format() const;

		VkImageSubresourceRange get_subresource_range() const;

		VkImageSubresourceLayers get_subresource_layers() const;

	private:
		Device& device;

		VkImageView handle{ VK_NULL_HANDLE };

		Image* image{VK_NULL_HANDLE};

		VkFormat format{};

		VkImageSubresourceRange subresource_range{};
	};
}        // namespace vkit
