#pragma once

#include <array>
#include <tuple>
#include <vulkan/vulkan.hpp>

#include "../Context.h"
#include "ShaderParameter.h"

namespace RTIDPRR {
namespace Graphics {
template <class... TShaderParameters>
class ShaderParameterGroup {
 public:
  ShaderParameterGroup(const vk::ShaderStageFlags stage,
                       TShaderParameters... values);

  static constexpr uint32_t sShaderParameterCount =
      std::tuple_size<std::tuple<TShaderParameters...>>::value;

  const vk::DescriptorSetLayout& getLayout() const { return mLayout; }
  const vk::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }

  virtual ~ShaderParameterGroup();

  std::tuple<TShaderParameters...> mParameters;

 private:
  vk::DescriptorPool mDescriptorPool;

  vk::DescriptorSet mDescriptorSet;
  vk::DescriptorSetLayout mLayout;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <size_t Iterator = 0, class T, class... Rest>
void initializeBindings(std::tuple<T, Rest...>& parameters,
                        std::vector<vk::DescriptorSetLayoutBinding>& bindings,
                        const vk::ShaderStageFlags stage) {
  vk::DescriptorSetLayoutBinding& layoutBinding = bindings[Iterator];
  layoutBinding.setBinding(Iterator)
      .setDescriptorType(T::getDescriptorType())
      .setDescriptorCount(1)
      .setStageFlags(stage);

  if constexpr (Iterator < sizeof...(Rest))
    initializeBindings<Iterator + 1>(parameters, bindings, stage);
}

template <size_t Iterator = 0, class T, class... Rest>
void bindAllParametersToGroup(std::tuple<T, Rest...>& parameters,
                              const vk::DescriptorSet& descriptorSet) {
  std::get<Iterator>(parameters).bindToGroup(descriptorSet, Iterator);
  if constexpr (Iterator < sizeof...(Rest))
    bindAllParametersToGroup<Iterator + 1>(parameters, descriptorSet);
}

template <size_t Iterator, vk::DescriptorType DescriptorType, class T,
          class... Rest>
uint32_t getDescriptorCountForType(std::tuple<T, Rest...>& parameters) {
  if constexpr (T::getDescriptorType() == DescriptorType) {
    if constexpr (Iterator < sizeof...(Rest))
      return 1 + getDescriptorCountForType<Iterator + 1, DescriptorType>(
                     parameters);
    else
      return 1;
  } else {
    if constexpr (Iterator < sizeof...(Rest))
      return getDescriptorCountForType<Iterator + 1, DescriptorType>(
          parameters);
    else
      return 0;
  }
}

template <class... TShaderParameters>
ShaderParameterGroup<TShaderParameters...>::ShaderParameterGroup(
    const vk::ShaderStageFlags stage, TShaderParameters... values)
    : mParameters(std::move(values)...) {
  const Device& device = Context::get().getDevice();
  constexpr uint32_t paramCount = sShaderParameterCount;

  std::vector<vk::DescriptorPoolSize> poolSizes;

  uint32_t uniformBufferCount =
      getDescriptorCountForType<0, vk::DescriptorType::eUniformBuffer>(
          mParameters);
  if (uniformBufferCount > 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize()
                               .setDescriptorCount(uniformBufferCount)
                               .setType(vk::DescriptorType::eUniformBuffer));
  }
  uint32_t combinedSamplerCount =
      getDescriptorCountForType<0, vk::DescriptorType::eCombinedImageSampler>(
          mParameters);
  if (combinedSamplerCount > 0) {
    poolSizes.emplace_back(
        vk::DescriptorPoolSize()
            .setDescriptorCount(combinedSamplerCount)
            .setType(vk::DescriptorType::eCombinedImageSampler));
  }

  vk::DescriptorPoolCreateInfo poolCreateInfo =
      vk::DescriptorPoolCreateInfo().setPoolSizes(poolSizes).setMaxSets(
          sShaderParameterCount);
  mDescriptorPool =
      device.getLogicalDeviceHandle().createDescriptorPool(poolCreateInfo);
  std::vector<vk::DescriptorSetLayoutBinding> bindingInfos(paramCount);
  initializeBindings(mParameters, bindingInfos, stage);

  vk::DescriptorSetLayoutCreateInfo layoutCreateInfo =
      vk::DescriptorSetLayoutCreateInfo()
          .setBindingCount(paramCount)
          .setBindings(bindingInfos);

  mLayout = device.getLogicalDeviceHandle().createDescriptorSetLayout(
      layoutCreateInfo);

  vk::DescriptorSetAllocateInfo descriptorAllocInfo =
      vk::DescriptorSetAllocateInfo()
          .setDescriptorPool(mDescriptorPool)
          .setDescriptorSetCount(1)
          .setSetLayouts(mLayout);
  mDescriptorSet = device.getLogicalDeviceHandle().allocateDescriptorSets(
      descriptorAllocInfo)[0];

  bindAllParametersToGroup(mParameters, mDescriptorSet);
};

template <class... TShaderParameters>
ShaderParameterGroup<TShaderParameters...>::~ShaderParameterGroup() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyDescriptorSetLayout(mLayout);
  device.getLogicalDeviceHandle().destroyDescriptorPool(mDescriptorPool);
}
