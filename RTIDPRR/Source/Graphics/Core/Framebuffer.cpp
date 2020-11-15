#include "Framebuffer.h"

using namespace RTIDPRR::Graphics;

Framebuffer::Framebuffer(const Device& device, const vk::RenderPass& renderPass,
                         const std::vector<vk::ImageView>& imageViews,
                         const uint32_t width, const uint32_t height) {
  vk::FramebufferCreateInfo framebufferCreateInfo =
      vk::FramebufferCreateInfo()
          .setRenderPass(renderPass)
          .setAttachments(imageViews)
          .setWidth(width)
          .setHeight(height)
          .setLayers(1);

  mFramebufferHandle =
      device.getLogicalDevice().createFramebuffer(framebufferCreateInfo);
}

Framebuffer::~Framebuffer() {}
