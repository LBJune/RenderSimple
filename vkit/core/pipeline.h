#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "render_pass.h"

namespace vkit
{
class Device;

class Pipeline
{
  public:
	Pipeline(Device &device);

	Pipeline(const Pipeline &) = delete;

	Pipeline(Pipeline &&other);

	virtual ~Pipeline();

	Pipeline &operator=(const Pipeline &) = delete;

	Pipeline &operator=(Pipeline &&) = delete;

	VkPipeline get_handle() const;

	void createRenderPass();

	//const PipelineState &get_state() const;

  protected:
	Device &device;

	VkPipeline handle = VK_NULL_HANDLE;

	//PipelineState state;

	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

	VkRenderPass render_pass{ VK_NULL_HANDLE };
};

class ShaderModule;
class GraphicsPipeline : public Pipeline
{
  public:
	GraphicsPipeline(GraphicsPipeline &&) = default;

	virtual ~GraphicsPipeline() = default;

	GraphicsPipeline(Device &        device, 
		std::vector<ShaderModule *>& shader_module
	);
};
}        // namespace vkit
