#pragma once

#include <vulkan/vulkan.hpp>

#include "../Texture.h"

namespace RTIDPRR {
namespace Graphics {
class ShaderParameterTexture {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eCombinedImageSampler;
  }

  ShaderParameterTexture(const Texture& texture);

  ShaderParameterTexture(ShaderParameterTexture&& other);

  void bindToGroup(const vk::DescriptorSet& descriptorSet,
                   const uint32_t binding);
  uint32_t getDescriptorCount() const { return 1; }

  virtual ~ShaderParameterTexture();

 private:
  vk::Sampler mSampler;
  const Texture& mTexture;
};
}  // namespace Graphics
}  // namespace RTIDPRR
