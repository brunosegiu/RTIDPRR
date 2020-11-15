#include "DeferredRenderer.h"

#include <stdlib.h>
using namespace RTIDPRR::Graphics;

DeferredRenderer::DeferredRenderer() {}

float random() { return float(abs(rand()) % 100000) / 100000.0f; }

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
      std::array<float, 4>{random(), random(), random(), 1.0f});

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass())
          .setFramebuffer(
              swapchain.getCurrentFramebuffer().getFramebufferHandle())
          .setRenderArea({vk::Offset2D{0, 0}, vk::Extent2D{1280, 720}})
          .setClearValues(vk::ClearValue{clearColor});

  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();

  swapchain.submitCommand(currentCommandBuffer);
}

DeferredRenderer::~DeferredRenderer() {}
