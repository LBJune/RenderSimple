#pragma once
#include <vulkan/vulkan.h>
#include "log.h"

namespace vkit
{

	/**
	 * @brief Helper function to convert a VkFormat enum to a string
	 * @param format Vulkan format to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkFormat format);

	/**
	 * @brief Helper function to convert a VkPresentModeKHR to a string
	 * @param present_mode Vulkan present mode to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkPresentModeKHR present_mode);

	/**
	 * @brief Helper function to convert a VkResult enum to a string
	 * @param result Vulkan result to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkResult result);

	/**
	 * @brief Helper function to convert a VkPhysicalDeviceType enum to a string
	 * @param type Vulkan physical device type to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkPhysicalDeviceType type);

	/**
	 * @brief Helper function to convert a VkSurfaceTransformFlagBitsKHR flag to a string
	 * @param transform_flag Vulkan surface transform flag bit to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkSurfaceTransformFlagBitsKHR transform_flag);

	/**
	 * @brief Helper function to convert a VkSurfaceFormatKHR format to a string
	 * @param surface_format Vulkan surface format to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkSurfaceFormatKHR surface_format);

	/**
	 * @brief Helper function to convert a VkCompositeAlphaFlagBitsKHR flag to a string
	 * @param composite_alpha Vulkan composite alpha flag bit to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkCompositeAlphaFlagBitsKHR composite_alpha);

	/**
	 * @brief Helper function to convert a VkImageUsageFlagBits flag to a string
	 * @param image_usage Vulkan image usage flag bit to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkImageUsageFlagBits image_usage);

	/**
	 * @brief Helper function to convert a VkExtent2D flag to a string
	 * @param format Vulkan format to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkExtent2D format);

	/**
	 * @brief Helper function to convert VkSampleCountFlagBits to a string
	 * @param flags Vulkan sample count flags to convert
	 * @return const char*
	 */
	const std::string to_string(VkSampleCountFlagBits flags);

	/**
	 * @brief Helper function to convert VkImageTiling to a string
	 * @param tiling Vulkan VkImageTiling to convert
	 * @return The string to return
	 */
	const std::string to_string(VkImageTiling tiling);

	/**
	 * @brief Helper function to convert VkImageType to a string
	 * @param type Vulkan VkImageType to convert
	 * @return The string to return
	 */
	const std::string to_string(VkImageType type);

	/**
	 * @brief Helper function to convert VkBlendFactor to a string
	 * @param blend Vulkan VkBlendFactor to convert
	 * @return The string to return
	 */
	const std::string to_string(VkBlendFactor blend);

	/**
	 * @brief Helper function to convert VkVertexInputRate to a string
	 * @param rate Vulkan VkVertexInputRate to convert
	 * @return The string to return
	 */
	const std::string to_string(VkVertexInputRate rate);

	/**
	 * @brief Helper function to convert VkBool32 to a string
	 * @param state Vulkan VkBool32 to convert
	 * @return The string to return
	 */
	const std::string to_string_vk_bool(VkBool32 state);

	/**
	 * @brief Helper function to convert VkPrimitiveTopology to a string
	 * @param topology Vulkan VkPrimitiveTopology to convert
	 * @return The string to return
	 */
	const std::string to_string(VkPrimitiveTopology topology);

	/**
	 * @brief Helper function to convert VkFrontFace to a string
	 * @param face Vulkan VkFrontFace to convert
	 * @return The string to return
	 */
	const std::string to_string(VkFrontFace face);

	/**
	 * @brief Helper function to convert VkPolygonMode to a string
	 * @param mode Vulkan VkPolygonMode to convert
	 * @return The string to return
	 */
	const std::string to_string(VkPolygonMode mode);

	/**
	 * @brief Helper function to convert VkCompareOp to a string
	 * @param operation Vulkan VkCompareOp to convert
	 * @return The string to return
	 */
	const std::string to_string(VkCompareOp operation);

	/**
	 * @brief Helper function to convert VkStencilOp to a string
	 * @param operation Vulkan VkStencilOp to convert
	 * @return The string to return
	 */
	const std::string to_string(VkStencilOp operation);

	/**
	 * @brief Helper function to convert VkLogicOp to a string
	 * @param operation Vulkan VkLogicOp to convert
	 * @return The string to return
	 */
	const std::string to_string(VkLogicOp operation);

	/**
	 * @brief Helper function to convert VkBlendOp to a string
	 * @param operation Vulkan VkBlendOp to convert
	 * @return The string to return
	 */
	const std::string to_string(VkBlendOp operation);

	/**
	 * @brief Helper function to convert bool to a string
	 * @param flag Vulkan bool to convert (true/false)
	 * @return The string to return
	 */
	const std::string to_string(bool flag);

}        // namespace vkit