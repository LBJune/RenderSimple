#include "pipeline.h"
#include "device.h"
#include "shader_module.h"
#include <utils/error.h>

namespace vkit
{
Pipeline::Pipeline(Device &device) :
    device{device}
{}

Pipeline::Pipeline(Pipeline &&other) :
    device{other.device},
    handle{other.handle}
{
	other.handle = VK_NULL_HANDLE;
}

Pipeline::~Pipeline()
{
	// Destroy pipeline
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device.get_handle(), handle, nullptr);
	}

	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device.get_handle(), pipelineLayout, nullptr);
	}

	if (render_pass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device.get_handle(), render_pass, nullptr);
	}

}

VkPipeline Pipeline::get_handle() const
{
	return handle;
}


void Pipeline::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device.get_handle(), &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

GraphicsPipeline::GraphicsPipeline(Device &        device,
	std::vector<ShaderModule *>& shader_modules
    ) :
    Pipeline{device}
{
	//1.shader stage
	std::vector<VkPipelineShaderStageCreateInfo> stage_create_infos;

	for (const ShaderModule * shader_module : shader_modules)
	{
		VkPipelineShaderStageCreateInfo stage_create_info{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};

		stage_create_info.stage = shader_module->get_stage();
		stage_create_info.pName = shader_module->get_entry_point().c_str();
		stage_create_info.module = shader_module->get_handle();
		stage_create_infos.push_back(stage_create_info);
	}

	//2.vertex input state
	VkPipelineVertexInputStateCreateInfo vertex_input_state{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	vertex_input_state.vertexBindingDescriptionCount = 0;
	vertex_input_state.vertexAttributeDescriptionCount = 0;

	//3.input assembly state
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	input_assembly_state.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state.primitiveRestartEnable = VK_FALSE;

	//4.viewport state
	VkPipelineViewportStateCreateInfo viewport_state{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;

	//5.rasterization state
	VkPipelineRasterizationStateCreateInfo rasterization_state{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	rasterization_state.depthClampEnable        = VK_FALSE;
	rasterization_state.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterization_state.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterization_state.frontFace               = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state.depthBiasEnable         = VK_FALSE;
	rasterization_state.depthBiasClamp          = 1.0f;
	rasterization_state.depthBiasSlopeFactor    = 1.0f;
	rasterization_state.lineWidth               = 1.0f;
	 
	//6.multisample state
	VkPipelineMultisampleStateCreateInfo multisample_state{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisample_state.sampleShadingEnable = VK_FALSE;
	multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	////7.depth stencil state
	//VkPipelineDepthStencilStateCreateInfo depth_stencil_state{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	//8. color blend state
	VkPipelineColorBlendStateCreateInfo color_blend_state{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	color_blend_state.logicOpEnable = VK_FALSE;
	color_blend_state.logicOp = VK_LOGIC_OP_COPY;
	color_blend_state.attachmentCount = 1;
	color_blend_state.pAttachments = &colorBlendAttachment;
	color_blend_state.blendConstants[0] = 0.0f;
	color_blend_state.blendConstants[1] = 0.0f;
	color_blend_state.blendConstants[2] = 0.0f;
	color_blend_state.blendConstants[3] = 0.0f;

	//9.dynamic states
	VkPipelineDynamicStateCreateInfo dynamic_state{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	std::vector<VkDynamicState> dynamic_states{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH,
		VK_DYNAMIC_STATE_DEPTH_BIAS,
		VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		VK_DYNAMIC_STATE_DEPTH_BOUNDS,
		VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
		VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
		VK_DYNAMIC_STATE_STENCIL_REFERENCE,
	};
	dynamic_state.pDynamicStates = dynamic_states.data();
	dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());

	//10.pipeline layout
	VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 0;
	pipeline_layout_create_info.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device.get_handle(), &pipeline_layout_create_info, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}


	//11.render pass
	createRenderPass();

	VkGraphicsPipelineCreateInfo create_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

	create_info.stageCount = static_cast<uint32_t>(stage_create_infos.size());
	create_info.pStages = stage_create_infos.data();
	create_info.pVertexInputState   = &vertex_input_state;
	create_info.pInputAssemblyState = &input_assembly_state;
	create_info.pViewportState      = &viewport_state;
	create_info.pRasterizationState = &rasterization_state;
	create_info.pMultisampleState   = &multisample_state;
	//create_info.pDepthStencilState  = &depth_stencil_state;
	create_info.pColorBlendState    = &color_blend_state;
	create_info.pDynamicState = &dynamic_state;

	create_info.layout     = pipelineLayout;
	create_info.renderPass = render_pass;
	create_info.subpass    = 0;

	auto result = vkCreateGraphicsPipelines(device.get_handle(), VK_NULL_HANDLE, 1, &create_info, nullptr, &handle);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create GraphicsPipelines" };
	}

	//for (auto shader_module : shader_modules)
	//{
	//	vkDestroyShaderModule(device.get_handle(), shader_module, nullptr);
	//}

	//state = pipeline_state;
}
}        // namespace vkit
