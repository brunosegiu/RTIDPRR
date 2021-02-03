#pragma once

#include <vulkan/vulkan.hpp>

#include "Texture.h"

namespace RTIDPRR {
namespace Graphics {
class RenderPass {
 public:
  RenderPass(const std::vector<const Texture*>& renderTargets,
             bool depthTestEnabled);
  RenderPass(vk::RenderPass renderPass, uint32_t colorAttachmentCount,
             bool depthTestEnabled);
  RenderPass(RenderPass&& other)
      : mHandle(std::move(other.mHandle)),
        mDepthTestEnabled(std::move(other.mDepthTestEnabled)),
        mColorAttachmentCount(std::move(other.mColorAttachmentCount)) {
    other.mHandle = nullptr;
  }

  const vk::RenderPass& getHandle() const { return mHandle; };

  bool getDepthTestEnabled() const { return mDepthTestEnabled; };
  uint32_t getColorAttachmentCount() const { return mColorAttachmentCount; };

  virtual ~RenderPass();

 private:
  vk::RenderPass mHandle;

  bool mDepthTestEnabled;
  uint32_t mColorAttachmentCount;
};
}  // namespace Graphics
}  // namespace RTIDPRR
