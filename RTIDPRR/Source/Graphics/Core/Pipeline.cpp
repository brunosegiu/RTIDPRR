#include "Pipeline.h"

using namespace RTIDPRR::Graphics;

Pipeline::Pipeline(const Device& device) {
	// Create render pass
	vk::AttachmentDescription colorAttachmentDescription =
		vk::AttachmentDescription()
		.setFormat(vk::Format::eUndefined)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorAttachmentRef =
		vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass =
		vk::SubpassDescription()
		.setColorAttachments(colorAttachmentRef);

	vk::RenderPassCreateInfo renderPassCreateInfo =
		vk::RenderPassCreateInfo()
		.setAttachments(colorAttachmentDescription)
		.setSubpasses(subpass);

	mRenderPassHandle = device.getLogicalDevice().createRenderPass(renderPassCreateInfo);

	// Setup vertex layout
	vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo =
		vk::PipelineVertexInputStateCreateInfo()
		.setPVertexBindingDescriptions(nullptr)
		.setPVertexAttributeDescriptions(nullptr);

	// Setup topology
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
		vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(false);

	// Setup viewport
	vk::Viewport viewport =
		vk::Viewport()
		.setX(0.0f)
		.setY(0.0f)
		.setWidth(1280.0f)
		.setHeight(720.0f)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);

	vk::Rect2D scissor =
		vk::Rect2D()
		.setOffset(0.0f)
		.setExtent(vk::Extent2D(1280, 720));

	vk::PipelineViewportStateCreateInfo viewportCreateInfo =
		vk::PipelineViewportStateCreateInfo()
		.setViewports(viewport)
		.setScissors(scissor);

	// Rasterization setup
	vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo =
		vk::PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(false)
		.setRasterizerDiscardEnable(false)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setLineWidth(1.0f)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eClockwise)
		.setDepthBiasEnable(false);

	// Multisampling state
	vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo =
		vk::PipelineMultisampleStateCreateInfo()
		.setSampleShadingEnable(false)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1);

	// Color blending state
	vk::PipelineColorBlendAttachmentState colorAttachmentCreateInfo =
		vk::PipelineColorBlendAttachmentState()
		.setColorWriteMask(
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA
		)
		.setBlendEnable(false);
	
	vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo =
		vk::PipelineColorBlendStateCreateInfo()
		.setLogicOpEnable(false)
		.setLogicOp(vk::LogicOp::eCopy)
		.setAttachments(colorAttachmentCreateInfo);

	// Shader parameter layout
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo =
		vk::PipelineLayoutCreateInfo()
		.setSetLayouts(nullptr)
		.setPushConstantRanges(nullptr);

	mLayoutHandle = device.getLogicalDevice().createPipelineLayout(pipelineLayoutCreateInfo);

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo =
		vk::GraphicsPipelineCreateInfo()
		.setStages(nullptr)
		.setPVertexInputState(&vertexInputCreateInfo)
		.setPInputAssemblyState(&inputAssemblyCreateInfo)
		.setPViewportState(&viewportCreateInfo)
		.setPRasterizationState(&rasterizerCreateInfo)
		.setPMultisampleState(&multisampleCreateInfo)
		.setPDepthStencilState(nullptr)
		.setPColorBlendState(&colorBlendCreateInfo)
		.setPDynamicState(nullptr)
		.setLayout(mLayoutHandle)
		.setRenderPass(mRenderPassHandle)
		.setSubpass(0);

	mPipelineHandle = device.getLogicalDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
}

Pipeline::~Pipeline() {

}