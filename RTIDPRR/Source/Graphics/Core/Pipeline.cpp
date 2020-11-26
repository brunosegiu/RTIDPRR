#include "Pipeline.h"

#include "../Geometry/IndexedVertexBuffer.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

Pipeline::Pipeline(
    const vk::RenderPass& renderPass, const Shader& vertexShader,
    const Shader& fragmentShader,
    const std::vector<vk::DescriptorSetLayout>& descriptorLayouts) {
  const Device& device = Context::get().getDevice();
  // Setup vertex layout
  vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo =
      vk::PipelineVertexInputStateCreateInfo()
          .setVertexBindingDescriptions(
              IndexedVertexBuffer::getBindingDescription())
          .setVertexAttributeDescriptions(
              IndexedVertexBuffer::getAttributeDescription());

  // Setup topology
  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo =
      vk::PipelineInputAssemblyStateCreateInfo()
          .setTopology(vk::PrimitiveTopology::eTriangleList)
          .setPrimitiveRestartEnable(false);

  // Setup viewport
  vk::Viewport viewport = vk::Viewport()
                              .setX(0.0f)
                              .setY(0.0f)
                              .setWidth(1280.0f)
                              .setHeight(720.0f)
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
  vk::PipelineColorBlendAttachmentState colorAttachmentCreateInfo =
      vk::PipelineColorBlendAttachmentState()
          .setColorWriteMask(
              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
          .setBlendEnable(false);

  vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo =
      vk::PipelineColorBlendStateCreateInfo()
          .setLogicOpEnable(false)
          .setLogicOp(vk::LogicOp::eCopy)
          .setAttachments(colorAttachmentCreateInfo);

  // Shader parameter layout
  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo =
      vk::PipelineLayoutCreateInfo()
          .setSetLayouts(descriptorLayouts)
          .setPushConstantRanges(nullptr);

  mLayoutHandle =
      device.getLogicalDevice().createPipelineLayout(pipelineLayoutCreateInfo);

  vk::PipelineShaderStageCreateInfo vertexStageCreateInfo =
      vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eVertex)
          .setModule(vertexShader.getModule())
          .setPName("main");

  vk::PipelineShaderStageCreateInfo fragmentStageCreateInfo =
      vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eFragment)
          .setModule(fragmentShader.getModule())
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

  mPipelineHandle = device.getLogicalDevice()
                        .createGraphicsPipeline(nullptr, pipelineCreateInfo)
                        .value;
}

Pipeline::~Pipeline() {}
