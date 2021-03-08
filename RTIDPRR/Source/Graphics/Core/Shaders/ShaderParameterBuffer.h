#pragma once

#include <array>
#include <tuple>
#include <vulkan/vulkan.hpp>

#include "../../../Misc/DebugUtils.h"
#include "../Buffer.h"
#include "../Context.h"

namespace RTIDPRR {
namespace Graphics {
template <typename ValueType>
class ShaderParameterBuffer {
 public:
  static constexpr vk::DescriptorType getDescriptorType() {
    return vk::DescriptorType::eStorageBuffer;
  }

  ShaderParameterBuffer(Buffer& buffer, size_t size);

  const vk::DescriptorSetLayout& getLayout() const { return mLayout; }
  const vk::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }

  virtual ~ShaderParameterBuffer();

 private:
  vk::DescriptorPool mDescriptorPool;

  vk::DescriptorSet mDescriptorSet;
  vk::DescriptorSetLayout mLayout;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <typename ValueType>
ShaderParameterBuffer<ValueType>::ShaderParameterBuffer(Buffer& buffer,
                                                        size_t size) {
  const Device& device = Context::get().getDevice();

  vk::DescriptorPoolSize poolSize =
      vk::DescriptorPoolSize().setDescriptorCount(1).setType(
          vk::DescriptorType::eStorageBuffer);

  vk::DescriptorPoolCreateInfo poolCreateInfo =
      vk::DescriptorPoolCreateInfo().setPoolSizes(poolSize).setMaxSets(1);
  mDescriptorPool = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createDescriptorPool(poolCreateInfo));
  vk::DescriptorSetLayoutBinding bindingInfo =
      vk::DescriptorSetLayoutBinding()
          .setBinding(0)
          .setDescriptorCount(1)
          .setDescriptorType(vk::DescriptorType::eStorageBuffer)
          .setStageFlags(vk::ShaderStageFlagBits::eCompute);

  vk::DescriptorSetLayoutCreateInfo layoutCreateInfo =
      vk::DescriptorSetLayoutCreateInfo().setBindings(bindingInfo);

  mLayout = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createDescriptorSetLayout(
          layoutCreateInfo));

  vk::DescriptorSetAllocateInfo descriptorAllocInfo =
      vk::DescriptorSetAllocateInfo()
          .setDescriptorPool(mDescriptorPool)
          .setDescriptorSetCount(1)
          .setSetLayouts(mLayout);

  mDescriptorSet =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().allocateDescriptorSets(
          descriptorAllocInfo))[0];

  vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo()
                                            .setBuffer(buffer.getBufferHandle())
                                            .setOffset(0)
                                            .setRange(size * sizeof(ValueType));

  vk::WriteDescriptorSet descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(mDescriptorSet)
          .setDstBinding(0)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eStorageBuffer)
          .setBufferInfo(bufferInfo);

  Context::get().getDevice().getLogicalDeviceHandle().updateDescriptorSets(
      descriptorWrite, nullptr);
};

template <typename ValueType>
ShaderParameterBuffer<ValueType>::~ShaderParameterBuffer() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyDescriptorSetLayout(mLayout);
  device.getLogicalDeviceHandle().destroyDescriptorPool(mDescriptorPool);
}
