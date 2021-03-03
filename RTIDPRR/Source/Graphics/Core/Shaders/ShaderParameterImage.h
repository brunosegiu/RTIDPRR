#pragma once

#include <vulkan/vulkan.hpp>

#include "../Texture.h"

namespace RTIDPRR {
namespace Graphics {

class ShaderParameterImage {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eStorageImage;
  }

  ShaderParameterImage(const Texture* texture);

  ShaderParameterImage(ShaderParameterImage&& other) noexcept;

  void bindToGroup(const vk::DescriptorSet& descriptorSet,
                   const uint32_t binding);

  uint32_t getDescriptorCount() const { return 1; }

  virtual ~ShaderParameterImage();

 private:
  const Texture* mTexture;
};
}  // namespace Graphics
}  // namespace RTIDPRR
