

#include "image_view.h"
#include "device.h"
#include "image.h"
#include <utils/error.h>
#include <utils/vk_common.h>

namespace vkit
{
	ImageView::ImageView( Image& img, VkImageViewType view_type, VkFormat format,
		uint32_t mip_level, uint32_t array_layer,
		uint32_t n_mip_levels, uint32_t n_array_layers) :
		device{ img.get_device() },
		image{ &img },
		format{ format }
	{
		if (format == VK_FORMAT_UNDEFINED)
		{
			this->format = format = image->get_format();
		}

		subresource_range.baseMipLevel = mip_level;
		subresource_range.baseArrayLayer = array_layer;
		subresource_range.levelCount = n_mip_levels == 0 ? image->get_subresource().mipLevel : n_mip_levels;
		subresource_range.layerCount = n_array_layers == 0 ? image->get_subresource().arrayLayer : n_array_layers;

		if (is_depth_format(format))
		{
			subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.image = image->get_handle();
		view_info.viewType = view_type;
		view_info.format = format;
		view_info.subresourceRange = subresource_range;

		if (vkCreateImageView(device.get_handle(), &view_info, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create ImageView!");
		}

		// Register this image view to its image
		// in order to be notified when it gets moved
		image->get_views().emplace(this);
	}

	ImageView::~ImageView()
	{
		if (handle != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device.get_handle(), handle, nullptr);
		}
	}

	VkImageView ImageView::get_handle() const
	{
		return handle;
	}

	const Image& ImageView::get_image() const
	{
		return *image;
	}

	void ImageView::set_image(Image& img)
	{
		image = &img;
	}

	VkFormat ImageView::get_format() const
	{
		return format;
	}

	VkImageSubresourceRange ImageView::get_subresource_range() const
	{
		return subresource_range;
	}

	VkImageSubresourceLayers ImageView::get_subresource_layers() const
	{
		VkImageSubresourceLayers subresource{};
		subresource.aspectMask = subresource_range.aspectMask;
		subresource.baseArrayLayer = subresource_range.baseArrayLayer;
		subresource.layerCount = subresource_range.layerCount;
		subresource.mipLevel = subresource_range.baseMipLevel;
		return subresource;
	}
}        // namespace vkit
