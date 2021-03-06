﻿#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace RTIDPRR {
namespace Graphics {
class Framebuffer {
 public:
  Framebuffer(const Device& device, const vk::RenderPass& renderPass,
              const std::vector<vk::ImageView>& imageViews,
              const uint32_t width, const uint32_t height);
  Framebuffer(Framebuffer&& other) noexcept;

  const vk::Framebuffer& getHandle() const { return mFramebufferHandle; };
  uint32_t getWidth() const { return mWidth; };
  uint32_t getHeight() const { return mHeight; };

  virtual ~Framebuffer();

 private:
  vk::Framebuffer mFramebufferHandle;

  uint32_t mWidth, mHeight;

  const Device& mDevice;
};
}  // namespace Graphics
}  // namespace RTIDPRR
