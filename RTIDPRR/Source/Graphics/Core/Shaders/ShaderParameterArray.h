#pragma once

#include <vulkan/vulkan.hpp>

#include "../Buffer.h"
#include "../Context.h"

namespace RTIDPRR {
namespace Graphics {
template <typename T>
class ShaderParameterArray {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eUniformBuffer;
  }

  ShaderParameterArray(size_t size);
  ShaderParameterArray(ShaderParameterArray&&) noexcept = default;

  void bindToGroup(const vk::DescriptorSet& descriptorSet,
                   const uint32_t binding);

  uint32_t getDescriptorCount() const {
    return static_cast<uint32_t>(mValues.size());
  }

  void update(const size_t& index, const T& value);

  virtual ~ShaderParameterArray() = default;

 private:
  vk::DeviceSize mSizePerEntry;
  std::vector<T> mValues;
  Buffer mBuffer;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <typename T>
vk::DeviceSize getSizePerEntry() {
  const vk::PhysicalDeviceProperties& properties =
      Context::get().getDevice().getDeviceProperties();
  vk::DeviceSize entrySize = sizeof(T);
  vk::DeviceSize minOffset = properties.limits.minUniformBufferOffsetAlignment;
  entrySize = (1 + entrySize / minOffset) * minOffset;
  return entrySize;
}

template <typename T>
ShaderParameterArray<T>::ShaderParameterArray(size_t size)
    : mSizePerEntry(getSizePerEntry<T>()),
      mValues(size),
      mBuffer(size * mSizePerEntry, vk::BufferUsageFlagBits::eUniformBuffer,
              vk::MemoryPropertyFlagBits::eHostVisible |
                  vk::MemoryPropertyFlagBits::eHostCoherent) {}

template <typename T>
void ShaderParameterArray<T>::bindToGroup(
    const vk::DescriptorSet& descriptorSet, const uint32_t binding) {
  std::vector<vk::DescriptorBufferInfo> bufferInfos(mValues.size());
  for (uint32_t index = 0; index < mValues.size(); ++index) {
    bufferInfos[index]
        .setBuffer(mBuffer.getBufferHandle())
        .setOffset(static_cast<uint32_t>(index * mSizePerEntry))
        .setRange(sizeof(T));
  }
  vk::WriteDescriptorSet descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(descriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(getDescriptorType())
          .setBufferInfo(bufferInfos);
  Context::get().getDevice().getLogicalDeviceHandle().updateDescriptorSets(
      descriptorWrite, nullptr);
}

template <typename T>
void ShaderParameterArray<T>::update(const size_t& index, const T& value) {
  mValues[index] = value;
  mBuffer.update(&value, index * mSizePerEntry, sizeof(T));
}
