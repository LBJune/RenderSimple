#include "image.h"
#include "device.h"

#include <utils/error.h>

namespace vkit
{
	inline VkImageType find_image_type(VkExtent3D extent)
	{
		VkImageType result{};

		uint32_t dim_num{ 0 };

		if (extent.width >= 1)
		{
			dim_num++;
		}

		if (extent.height >= 1)
		{
			dim_num++;
		}

		if (extent.depth > 1)
		{
			dim_num++;
		}

		switch (dim_num)
		{
		case 1:
			result = VK_IMAGE_TYPE_1D;
			break;
		case 2:
			result = VK_IMAGE_TYPE_2D;
			break;
		case 3:
			result = VK_IMAGE_TYPE_3D;
			break;
		default:
			throw std::runtime_error("No image type found.");
			break;
		}

		return result;
	}

	Image::Image(vkit::Device& device,
		VkImage               handle,
		const VkExtent3D& extent,
		VkFormat              format,
		VkImageUsageFlags     image_usage,
		VkSampleCountFlagBits sample_count):
		device{ device },
		handle{ handle }
	{
		create_info.extent = extent;
		create_info.imageType = find_image_type(extent);
		create_info.format = format;
		create_info.usage = image_usage;
		create_info.samples = sample_count;
		subresource.arrayLayer = create_info.arrayLayers = 1;
		subresource.mipLevel = create_info.mipLevels = 1;
	}

	Image::Image(
		vkit::Device& device,
		const VkExtent3D& extent,
		VkFormat              format,
		VkImageUsageFlags     image_usage,
		VkSampleCountFlagBits sample_count,
		uint32_t              mip_levels,
		uint32_t              array_layers,
		VkImageTiling         tiling,
		VkImageCreateFlags    flags,
		uint32_t              num_queue_families,
		const uint32_t* queue_families,
		const void *pNext,
		VkImageLayout         initialLayout):
		device{ device }
	{
		create_info.pNext = pNext;
		create_info.flags = flags;
		create_info.imageType = find_image_type(extent);
		create_info.format = format;
		create_info.extent = extent;
		create_info.mipLevels = mip_levels;
		create_info.arrayLayers = array_layers;
		create_info.samples = sample_count;
		create_info.tiling = tiling;
		create_info.usage = image_usage;
		create_info.sharingMode = (1 < num_queue_families) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = num_queue_families;
		create_info.pQueueFamilyIndices = queue_families;
		create_info.initialLayout = initialLayout;

		if (vkCreateImage(device.get_handle(), &create_info, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		subresource.arrayLayer = create_info.arrayLayers;
		subresource.mipLevel = create_info.mipLevels;

		is_new_alloc = true;
	}

	Image::~Image()
	{
		if (handle != VK_NULL_HANDLE && is_new_alloc)
		{
			vkDestroyImage(device.get_handle(), handle, nullptr);
		}
	}

	Device& Image::get_device()
	{
		return device;
	}

	VkImage Image::get_handle() const
	{
		return handle;
	}

	VkImageType Image::get_type() const
	{
		return create_info.imageType;
	}

	const VkExtent3D& Image::get_extent() const
	{
		return create_info.extent;
	}

	VkFormat Image::get_format() const
	{
		return create_info.format;
	}

	VkSampleCountFlagBits Image::get_sample_count() const
	{
		return create_info.samples;
	}

	VkImageUsageFlags Image::get_usage() const 
	{
		return create_info.usage;
	}

	VkImageTiling Image::get_tiling() const
	{
		return create_info.tiling;
	}

	const VkImageSubresource& Image::get_subresource() const
	{
		return subresource;
	}

	uint32_t Image::get_array_layer_count() const
	{
		return create_info.arrayLayers;
	}

	std::unordered_set<ImageView*>& Image::get_views()
	{
		return views;
	}

}        // namespace vkit
