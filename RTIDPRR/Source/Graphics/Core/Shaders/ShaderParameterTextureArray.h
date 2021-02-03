#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "../Texture.h"
#include "ShaderParameterTexture.h"

namespace RTIDPRR {
namespace Graphics {
class ShaderParameterTextureArray {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eCombinedImageSampler;
  }

  ShaderParameterTextureArray(
      const std::vector<SamplerOptions>& samplerOptions);

  ShaderParameterTextureArray(ShaderParameterTextureArray&& other);

  void bindToGroup(const vk::DescriptorSet& descriptorSet,
                   const uint32_t binding);
  uint32_t getDescriptorCount() const {
    return static_cast<uint32_t>(mTextures.size());
  }

  virtual ~ShaderParameterTextureArray();

 private:
  std::vector<vk::Sampler> mSamplers;
  std::vector<Texture const*> mTextures;
};
}  // namespace Graphics
}  // namespace RTIDPRR
