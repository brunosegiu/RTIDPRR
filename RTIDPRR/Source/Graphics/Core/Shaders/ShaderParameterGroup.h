﻿#pragma once

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
  ShaderParameterGroup(const vk::ShaderStageFlags stage);

  static constexpr uint32_t sShaderParameterCount =
      std::tuple_size<std::tuple<TShaderParameters...>>::value;

  const vk::DescriptorSetLayout& getLayout() const { return mLayout; }
  const vk::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }

  virtual ~ShaderParameterGroup() = default;

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

  if constexpr (Iterator != sizeof...(Rest))
    initializeBindings<Iterator + 1>(parameters, bindings, stage);
}

template <size_t Iterator = 0, class T, class... Rest>
void bindAllParametersToGroup(std::tuple<T, Rest...>& parameters,
                              const vk::DescriptorSet& descriptorSet) {
  std::get<Iterator>(parameters).bindToGroup(descriptorSet, Iterator);
  if constexpr (Iterator != sizeof...(Rest))
    bindAllParametersToGroup<Iterator + 1>(parameters, descriptorSet);
}

template <class... TShaderParameters>
ShaderParameterGroup<TShaderParameters...>::ShaderParameterGroup(
    const vk::ShaderStageFlags stage)
    : mParameters() {
  const Device& device = Context::get().getDevice();
  constexpr uint32_t paramCount = sShaderParameterCount;

  vk::DescriptorPoolSize poolSize =
      vk::DescriptorPoolSize().setDescriptorCount(1);
  vk::DescriptorPoolCreateInfo poolCreateInfo =
      vk::DescriptorPoolCreateInfo().setPoolSizes(poolSize).setMaxSets(1);
  mDescriptorPool =
      device.getLogicalDevice().createDescriptorPool(poolCreateInfo);
  std::vector<vk::DescriptorSetLayoutBinding> bindingInfos(paramCount);
  initializeBindings(mParameters, bindingInfos, stage);

  vk::DescriptorSetLayoutCreateInfo layoutCreateInfo =
      vk::DescriptorSetLayoutCreateInfo()
          .setBindingCount(paramCount)
          .setBindings(bindingInfos);

  mLayout =
      device.getLogicalDevice().createDescriptorSetLayout(layoutCreateInfo);

  vk::DescriptorSetAllocateInfo descriptorAllocInfo =
      vk::DescriptorSetAllocateInfo()
          .setDescriptorPool(mDescriptorPool)
          .setDescriptorSetCount(1)
          .setSetLayouts(mLayout);
  mDescriptorSet =
      device.getLogicalDevice().allocateDescriptorSets(descriptorAllocInfo)[0];

  bindAllParametersToGroup(mParameters, mDescriptorSet);
};
