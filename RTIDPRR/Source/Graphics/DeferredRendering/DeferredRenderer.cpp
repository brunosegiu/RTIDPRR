#include "DeferredRenderer.h"

using namespace RTIDPRR::Graphics;

DeferredRenderer::DeferredRenderer()
    : mPipeline(Context::get().getSwapchain().getMainRenderPass(),
                *Shader::loadShader("Source/Shaders/Build/test.vert"),
                *Shader::loadShader("Source/Shaders/Build/test.frag"))

{}

void DeferredRenderer::render() {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();
  swapchain.swapBuffers();

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

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass())
          .setFramebuffer(
              swapchain.getCurrentFramebuffer().getFramebufferHandle())
          .setRenderArea({vk::Offset2D{0, 0}, vk::Extent2D{1280, 720}})
          .setClearValues(vk::ClearValue{clearColor});

  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  currentCommandBuffer.bindPipeline(
      vk::PipelineBindPoint::eGraphics, mPipeline.getPipelineHandle());

  currentCommandBuffer.draw(3, 1, 0, 0);

  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();

  swapchain.submitCommand(currentCommandBuffer);
}

DeferredRenderer::~DeferredRenderer() {}
