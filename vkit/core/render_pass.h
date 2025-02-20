#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <utils//vk_common.h>

namespace vkit
{
class Device;

class RenderPass
{
  public:
	RenderPass(Device                           &device,
	           const std::vector<Attachment>    &attachments,
	           const std::vector<LoadStoreInfo> &load_store_infos,
	           const std::vector<SubpassInfo>   &subpasses);

	RenderPass(const RenderPass &) = delete;

	RenderPass(RenderPass &&other);

	~RenderPass();

	RenderPass &operator=(const RenderPass &) = delete;

	RenderPass &operator=(RenderPass &&) = delete;

	Device& get_device();

	VkRenderPass get_handle() const;

	const uint32_t get_color_output_count(uint32_t subpass_index) const;

	const VkExtent2D get_render_area_granularity() const;

  private:
	Device& device;

	VkRenderPass handle{ VK_NULL_HANDLE };

	size_t subpass_count;

	template <typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference, typename T_SubpassDependency, typename T_RenderPassCreateInfo>
	void create_renderpass(const std::vector<Attachment> &attachments, const std::vector<LoadStoreInfo> &load_store_infos, const std::vector<SubpassInfo> &subpasses);

	std::vector<uint32_t> color_output_count;
};
}        // namespace vkit
