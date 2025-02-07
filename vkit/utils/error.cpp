#include "error.h"

namespace vkr
{
VulkanException::VulkanException(const VkResult result, const std::string &msg) :
    result{result},
    std::runtime_error{msg}
{
	error_message = std::string(std::runtime_error::what()) + std::string{" : "} + std::to_string(result);
}

const char *VulkanException::what() const noexcept
{
	return error_message.c_str();
}
}        // namespace vkr
