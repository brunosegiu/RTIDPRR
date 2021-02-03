#pragma once

#include <vulkan/vulkan.hpp>

#include "../Texture.h"

namespace RTIDPRR {
namespace Graphics {
struct SamplerOptions {
  Texture const* texture = nullptr;
  vk::Filter filter = vk::Filter::eNearest;
  vk::SamplerAddressMode clampMode = vk::SamplerAddressMode::eClampToEdge;
  vk::BorderColor borderColor = vk::BorderColor::eFloatOpaqueWhite;

  SamplerOptions& setTexture(const Texture* texture) {
    this->texture = texture;
    return *this;
  }
  SamplerOptions& setFilter(vk::Filter filter) {
    this->filter = filter;
    return *this;
  }
  SamplerOptions& setClampMode(vk::SamplerAddressMode clampMode) {
    this->clampMode = clampMode;
    return *this;
  }
  SamplerOptions& setBorderColor(vk::BorderColor borderColor) {
    this->borderColor = borderColor;
    return *this;
  }
};

class ShaderParameterTexture {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eCombinedImageSampler;
  }

  ShaderParameterTexture(const SamplerOptions& samplerOptions);

  ShaderParameterTexture(ShaderParameterTexture&& other);

  void bindToGroup(const vk::DescriptorSet& descriptorSet,
                   const uint32_t binding);
  uint32_t getDescriptorCount() const { return 1; }

  virtual ~ShaderParameterTexture();

 private:
  vk::Sampler mSampler;
  const Texture* mTexture;
};
}  // namespace Graphics
}  // namespace RTIDPRR
