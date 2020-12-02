#include "Framebuffer.h"

using namespace RTIDPRR::Graphics;

Framebuffer::Framebuffer(const Device& device, const vk::RenderPass& renderPass,
                         const std::vector<vk::ImageView>& imageViews,
                         const uint32_t width, const uint32_t height)
    : mDevice(device) {
  vk::FramebufferCreateInfo framebufferCreateInfo =
      vk::FramebufferCreateInfo()
          .setRenderPass(renderPass)
          .setAttachments(imageViews)
          .setWidth(width)
          .setHeight(height)
          .setLayers(1);

  mFramebufferHandle =
      device.getLogicalDeviceHandle().createFramebuffer(framebufferCreateInfo);
}

Framebuffer::Framebuffer(Framebuffer&& other)
    : mFramebufferHandle(std::move(other.mFramebufferHandle)),
      mDevice(std::move(other.mDevice)) {
  other.mFramebufferHandle = nullptr;
}

Framebuffer::~Framebuffer() {
  mDevice.getLogicalDeviceHandle().destroyFramebuffer(mFramebufferHandle);
}
