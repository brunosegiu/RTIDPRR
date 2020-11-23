#include "DeferredRenderer.h"

using namespace RTIDPRR::Graphics;

DeferredRenderer::DeferredRenderer()
    : mShaderParameters(vk::ShaderStageFlagBits::eFragment),
      mPipeline(
          Context::get().getSwapchain().getMainRenderPass(),
          *Shader::loadShader("Source/Shaders/Build/test.vert"),
          *Shader::loadShader("Source/Shaders/Build/test.frag"),
          std::vector<vk::DescriptorSetLayout>{mShaderParameters.getLayout()})

{}

void DeferredRenderer::render() {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();
  swapchain.swapBuffers();

  static glm::vec4 a{};
  a += glm::vec4(0, 0, 0.01f, 0.0f);
  if (a.b > 1) a = glm::vec4();
  std::get<0>(mShaderParameters.mParameters).update(a);

  vk::CommandBufferAllocateInfo commandAllocInfo =
      vk::CommandBufferAllocateInfo()
          .setCommandPool(graphicsQueue.getCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1);

  std::vector<vk::CommandBuffer> commandBuffers =
      device.getLogicalDevice().allocateCommandBuffers(commandAllocInfo);

  vk::CommandBuffer& currentCommandBuffer = commandBuffers[0];

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  currentCommandBuffer.begin(beginInfo);

  const vk::ClearColorValue clearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});

  const std::vector<vk::ClearValue> clearValues{clearColor};

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass())
          .setFramebuffer(
              swapchain.getCurrentFramebuffer().getFramebufferHandle())
          .setRenderArea({vk::Offset2D{0, 0}, vk::Extent2D{1280, 720}})
          .setClearValues(clearValues);

  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    mPipeline.getPipelineHandle());

  currentCommandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, mPipeline.getPipelineLayout(), 0,
      mShaderParameters.getDescriptorSet(), nullptr);

  currentCommandBuffer.draw(3, 1, 0, 0);

  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();

  swapchain.submitCommand(currentCommandBuffer);
}

DeferredRenderer::~DeferredRenderer() {}
