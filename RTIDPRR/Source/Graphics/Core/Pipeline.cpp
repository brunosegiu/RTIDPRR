#include "Pipeline.h"

#include "../../Misc/DebugUtils.h"
#include "../Geometry/IndexedVertexBuffer.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

Pipeline::Pipeline(
    const RenderPass& renderPass, const vk::Extent2D& extent,
    const GeometryLayout& geometryLayout,
    const std::vector<std::string>& shaderPaths,
    const std::vector<vk::DescriptorSetLayout>& descriptorLayouts,
    const std::vector<vk::PushConstantRange>& pushConstants,
    const PipelineCreateOptions& options) {
  const Device& device = Context::get().getDevice();

  // Setup geometry state
  vk::VertexInputBindingDescription vertexBindingDesc =
      geometryLayout.getBindingDescription();
  vk::VertexInputAttributeDescription vertexAttrDesc =
      geometryLayout.getAttributeDescription();
  vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo =
      geometryLayout != GeometryLayout::None
          ? vk::PipelineVertexInputStateCreateInfo()
                .setVertexBindingDescriptions(vertexBindingDesc)
                .setVertexAttributeDescriptions(vertexAttrDesc)
          : vk::PipelineVertexInputStateCreateInfo();

  // Setup topology
  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
      vk::PipelineInputAssemblyStateCreateInfo()
          .setTopology(vk::PrimitiveTopology::eTriangleList)
          .setPrimitiveRestartEnable(false);

  // Setup viewport
  vk::Viewport viewport = vk::Viewport()
                              .setX(0.0f)
                              .setY(0.0f)
                              .setWidth(static_cast<float>(extent.width))
                              .setHeight(static_cast<float>(extent.height))
                              .setMinDepth(0.0f)
                              .setMaxDepth(1.0f);

  vk::Rect2D scissor = vk::Rect2D().setOffset(0).setExtent(extent);

  vk::PipelineViewportStateCreateInfo viewportCreateInfo =
      vk::PipelineViewportStateCreateInfo().setViewports(viewport).setScissors(
          scissor);

  // Rasterization setup
  vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo =
      vk::PipelineRasterizationStateCreateInfo()
          .setDepthClampEnable(false)
          .setRasterizerDiscardEnable(false)
          .setPolygonMode(vk::PolygonMode::eFill)
          .setLineWidth(1.0f)
          .setCullMode(options.cullMode)
          .setFrontFace(vk::FrontFace::eCounterClockwise)
          .setDepthBiasEnable(options.enableDepthBias);

  // Multisampling state
  vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo =
      vk::PipelineMultisampleStateCreateInfo()
          .setSampleShadingEnable(false)
          .setRasterizationSamples(vk::SampleCountFlagBits::e1);

  // Color blending state
  vk::PipelineColorBlendAttachmentState baseAttachmentCreateInfo =
      vk::PipelineColorBlendAttachmentState()
          .setColorWriteMask(
              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
          .setBlendEnable(false);

  std::vector<vk::PipelineColorBlendAttachmentState> blendAttachments(
      renderPass.getColorAttachmentCount(), baseAttachmentCreateInfo);

  vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo =
      vk::PipelineColorBlendStateCreateInfo()
          .setLogicOpEnable(false)
          .setLogicOp(vk::LogicOp::eCopy)
          .setAttachments(blendAttachments);

  // Shader parameter layout
  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo =
      vk::PipelineLayoutCreateInfo()
          .setSetLayouts(descriptorLayouts)
          .setPushConstantRanges(pushConstants);

  // Depth testing
  vk::PipelineDepthStencilStateCreateInfo depthStencilState =
      vk::PipelineDepthStencilStateCreateInfo{}
          .setDepthTestEnable(true)
          .setDepthWriteEnable(true)
          .setDepthCompareOp(vk::CompareOp::eLess)
          .setDepthBoundsTestEnable(false)
          .setStencilTestEnable(false);

  mLayoutHandle =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createPipelineLayout(
          pipelineLayoutCreateInfo));

  // Load and bind shaders
  mShaders.reserve(shaderPaths.size());
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
  shaderStages.reserve(shaderPaths.size());
  for (const std::string& shaderPath : shaderPaths) {
    static const char* entryPointName = "main";
    Shader* shader = mShaders.emplace_back(Shader::loadShader(shaderPath));
    shaderStages.emplace_back(vk::PipelineShaderStageCreateInfo()
                                  .setStage(shader->getStage())
                                  .setModule(shader->getModule())
                                  .setPName(entryPointName));
  }

  const std::vector<vk::DynamicState> dynamicStates{
      vk::DynamicState::eViewport};
  vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo =
      vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamicStates);

  vk::GraphicsPipelineCreateInfo pipelineCreateInfo =
      vk::GraphicsPipelineCreateInfo()
          .setStages(shaderStages)
          .setPVertexInputState(&vertexInputCreateInfo)
          .setPInputAssemblyState(&inputAssemblyCreateInfo)
          .setPViewportState(&viewportCreateInfo)
          .setPRasterizationState(&rasterizerCreateInfo)
          .setPMultisampleState(&multisampleCreateInfo)
          .setPDepthStencilState(
              renderPass.getDepthTestEnabled() ? &depthStencilState : nullptr)
          .setPColorBlendState(&colorBlendCreateInfo)
          .setPDynamicState(&dynamicStateCreateInfo)
          .setLayout(mLayoutHandle)
          .setRenderPass(renderPass.getHandle())
          .setSubpass(0);

  mPipelineHandle =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createGraphicsPipeline(
          nullptr, pipelineCreateInfo));
}

Pipeline::~Pipeline() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyPipelineLayout(mLayoutHandle);
  device.getLogicalDeviceHandle().destroyPipeline(mPipelineHandle);
  for (Shader* shader : mShaders) {
    delete shader;
  }
}
