#include "BasePassPipeline.h"

#include "../Core/Context.h"
#include "../Geometry/IndexedVertexBuffer.h"

using namespace RTIDPRR::Graphics;

BasePassPipeline::BasePassPipeline(
    const vk::RenderPass& renderPass,
    const std::vector<vk::DescriptorSetLayout> descriptorLayouts,
    const vk::Extent2D extent)
    : mVertexShader(Shader::loadShader("Source/Shaders/Build/BasePass.vert")),
      mFragmentShader(
          Shader::loadShader("Source/Shaders/Build/BasePass.frag")) {
  const Device& device = Context::get().getDevice();
  // Setup vertex layout
  const std::vector<vk::VertexInputBindingDescription> bindingDescription{
      IndexedVertexBuffer::getBindingDescription()};
  const std::vector<vk::VertexInputAttributeDescription> attributeDescription{
      IndexedVertexBuffer::getAttributeDescription()};
  vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo =
      vk::PipelineVertexInputStateCreateInfo()
          .setVertexBindingDescriptions(bindingDescription)
          .setVertexAttributeDescriptions(attributeDescription);

  // Setup topology
  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
      vk::PipelineInputAssemblyStateCreateInfo()
          .setTopology(vk::PrimitiveTopology::eTriangleList)
          .setPrimitiveRestartEnable(false);

  // Setup viewport
  vk::Viewport viewport = vk::Viewport()
                              .setX(0.0f)
                              .setY(0.0f)
                              .setWidth(extent.width)
                              .setHeight(extent.height)
                              .setMinDepth(0.0f)
                              .setMaxDepth(1.0f);

  vk::Rect2D scissor =
      vk::Rect2D().setOffset(0).setExtent(vk::Extent2D(1280, 720));

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
          .setCullMode(vk::CullModeFlagBits::eBack)
          .setFrontFace(vk::FrontFace::eClockwise)
          .setDepthBiasEnable(false);

  // Multisampling state
  vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo =
      vk::PipelineMultisampleStateCreateInfo()
          .setSampleShadingEnable(false)
          .setRasterizationSamples(vk::SampleCountFlagBits::e1);

  // Color blending state
  std::vector<vk::PipelineColorBlendAttachmentState> colorAttachmentCreateInfos{
      vk::PipelineColorBlendAttachmentState()
          .setColorWriteMask(
              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
          .setBlendEnable(false),
      vk::PipelineColorBlendAttachmentState()
          .setColorWriteMask(
              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
          .setBlendEnable(false)};

  vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo =
      vk::PipelineColorBlendStateCreateInfo()
          .setLogicOpEnable(false)
          .setLogicOp(vk::LogicOp::eCopy)
          .setAttachments(colorAttachmentCreateInfos);

  // Depth state
  vk::PipelineDepthStencilStateCreateInfo depthStencilState =
      vk::PipelineDepthStencilStateCreateInfo()
          .setDepthTestEnable(true)
          .setDepthWriteEnable(true)
          .setDepthCompareOp(vk::CompareOp::eLess)
          .setDepthBoundsTestEnable(false)
          .setStencilTestEnable(false);

  // Shader parameter layout
  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo =
      vk::PipelineLayoutCreateInfo()
          .setSetLayouts(descriptorLayouts)
          .setPushConstantRanges(nullptr);

  mLayoutHandle = device.getLogicalDeviceHandle().createPipelineLayout(
      pipelineLayoutCreateInfo);

  vk::PipelineShaderStageCreateInfo vertexStageCreateInfo =
      vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eVertex)
          .setModule(mVertexShader->getModule())
          .setPName("main");

  vk::PipelineShaderStageCreateInfo fragmentStageCreateInfo =
      vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eFragment)
          .setModule(mFragmentShader->getModule())
          .setPName("main");

  const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{
      vertexStageCreateInfo, fragmentStageCreateInfo};

  vk::GraphicsPipelineCreateInfo pipelineCreateInfo =
      vk::GraphicsPipelineCreateInfo()
          .setStages(shaderStages)
          .setPVertexInputState(&vertexInputCreateInfo)
          .setPInputAssemblyState(&inputAssemblyCreateInfo)
          .setPViewportState(&viewportCreateInfo)
          .setPRasterizationState(&rasterizerCreateInfo)
          .setPMultisampleState(&multisampleCreateInfo)
          .setPDepthStencilState(nullptr)
          .setPColorBlendState(&colorBlendCreateInfo)
          .setPDynamicState(nullptr)
          .setLayout(mLayoutHandle)
          .setRenderPass(renderPass)
          .setSubpass(0);

  mPipelineHandle = device.getLogicalDeviceHandle()
                        .createGraphicsPipeline(nullptr, pipelineCreateInfo)
                        .value;
}

BasePassPipeline::~BasePassPipeline() {
  delete mVertexShader;
  delete mFragmentShader;
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyPipelineLayout(mLayoutHandle);
  device.getLogicalDeviceHandle().destroyPipeline(mPipelineHandle);
}
