#pragma once

#include <cassert>
#include <stdexcept>
#include <string>

#include <vulkan/vulkan_core.h>
#include "log.h"

namespace vkit
{
/**
 * @brief Vulkan exception structure
 */
class VulkanException : public std::runtime_error
{
  public:
	/**
	 * @brief Vulkan exception constructor
	 */
	VulkanException(VkResult result, const std::string &msg = "Vulkan error");

	/**
	 * @brief Returns the Vulkan error code as string
	 * @return String message of exception
	 */
	const char *what() const noexcept override;

	VkResult result;

  private:
	std::string error_message;
};
}        // namespace vkit

/// @brief Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x)                                                                    \
	do                                                                                 \
	{                                                                                  \
		VkResult err = x;                                                              \
		if (err)                                                                       \
		{                                                                              \
			throw std::runtime_error("Detected Vulkan error: " + std::to_string(err)); \
		}                                                                              \
	} while (0)

#define ASSERT_VK_HANDLE(handle)        \
	do                                  \
	{                                   \
		if ((handle) == VK_NULL_HANDLE) \
		{                               \
			LOGE("Handle is NULL");     \
			abort();                    \
		}                               \
	} while (0)
