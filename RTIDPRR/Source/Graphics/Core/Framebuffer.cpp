#include "Framebuffer.h"

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

Framebuffer::Framebuffer(const Device& device, const vk::RenderPass& renderPass,
                         const std::vector<vk::ImageView>& imageViews,
                         const uint32_t width, const uint32_t height)
    : mDevice(device), mWidth(width), mHeight(height) {
  vk::FramebufferCreateInfo framebufferCreateInfo =
      vk::FramebufferCreateInfo()
          .setRenderPass(renderPass)
          .setAttachments(imageViews)
          .setWidth(width)
          .setHeight(height)
          .setLayers(1);

  mFramebufferHandle = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createFramebuffer(framebufferCreateInfo));
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : mFramebufferHandle(std::move(other.mFramebufferHandle)),
      mDevice(other.mDevice),
      mWidth(other.mWidth),
      mHeight(other.mHeight) {
  other.mFramebufferHandle = nullptr;
}

Framebuffer::~Framebuffer() {
  mDevice.getLogicalDeviceHandle().destroyFramebuffer(mFramebufferHandle);
}
