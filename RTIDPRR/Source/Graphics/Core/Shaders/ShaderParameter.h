﻿#pragma once

#include <vulkan/vulkan.hpp>

#include "../Buffer.h"
#include "../Context.h"

namespace RTIDPRR {
namespace Graphics {
template <typename T>
class ShaderParameter {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eUniformBuffer;
  }

  ShaderParameter();
  ShaderParameter(ShaderParameter&&) noexcept = default;

  void bindToGroup(const vk::DescriptorSet& descriptorSet,
                   const uint32_t binding);
  uint32_t getDescriptorCount() const { return 1; }

  void update(const T& value);

  virtual ~ShaderParameter() = default;

 private:
  T mValue;
  Buffer mBuffer;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <typename T>
ShaderParameter<T>::ShaderParameter()
    : mValue(),
      mBuffer(sizeof(T), vk::BufferUsageFlagBits::eUniformBuffer,
              vk::MemoryPropertyFlagBits::eHostVisible |
                  vk::MemoryPropertyFlagBits::eHostCoherent) {}

template <typename T>
void ShaderParameter<T>::bindToGroup(const vk::DescriptorSet& descriptorSet,
                                     const uint32_t binding) {
  vk::DescriptorBufferInfo bufferInfo =
      vk::DescriptorBufferInfo()
          .setBuffer(mBuffer.getBufferHandle())
          .setOffset(0)
          .setRange(static_cast<uint32_t>(sizeof(T)));
  vk::WriteDescriptorSet descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(descriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(getDescriptorType())
          .setBufferInfo(bufferInfo);
  Context::get().getDevice().getLogicalDeviceHandle().updateDescriptorSets(
      descriptorWrite, nullptr);
}

template <typename T>
void ShaderParameter<T>::update(const T& value) {
  mValue = value;
  mBuffer.update(&mValue);
}
