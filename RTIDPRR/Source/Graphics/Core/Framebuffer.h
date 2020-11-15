#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace RTIDPRR {
namespace Graphics {
class Framebuffer {
 public:
  Framebuffer(const Device& device, const vk::RenderPass& renderPass,
              const std::vector<vk::ImageView>& imageViews,
              const uint32_t width, const uint32_t height);

  const vk::Framebuffer& getFramebufferHandle() const {
    return mFramebufferHandle;
  };

  virtual ~Framebuffer();

 private:
  vk::Framebuffer mFramebufferHandle;
};
}  // namespace Graphics
}  // namespace RTIDPRR
