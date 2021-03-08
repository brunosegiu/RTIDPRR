#pragma once

#include <array>
#include <tuple>
#include <vulkan/vulkan.hpp>

#include "../Context.h"

namespace RTIDPRR {
namespace Graphics {
template <class... TShaderParameters>
class ShaderParameterGroup {
 public:
  ShaderParameterGroup(const vk::ShaderStageFlags stage,
                       TShaderParameters... values);

  ShaderParameterGroup(ShaderParameterGroup&& other) noexcept;

  static constexpr uint32_t sShaderParameterCount =
      std::tuple_size<std::tuple<TShaderParameters...>>::value;

  const vk::DescriptorSetLayout& getLayout() const { return mLayout; }
  const vk::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }

  virtual ~ShaderParameterGroup();

  std::tuple<TShaderParameters...>& getShaderParameters() {
    return mParameters;
  };

 private:
  std::tuple<TShaderParameters...> mParameters;

  vk::DescriptorPool mDescriptorPool;

  vk::DescriptorSet mDescriptorSet;
  vk::DescriptorSetLayout mLayout;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <size_t Iterator = 0, class T, class... Rest>
void initializeBindings(const std::tuple<T, Rest...>& parameters,
                        std::vector<vk::DescriptorSetLayoutBinding>& bindings,
                        const vk::ShaderStageFlags stage) {
  vk::DescriptorSetLayoutBinding& layoutBinding = bindings[Iterator];
  layoutBinding.setBinding(Iterator)
      .setDescriptorType(std::get<Iterator>(parameters).getDescriptorType())
      .setDescriptorCount(std::get<Iterator>(parameters).getDescriptorCount())
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
uint32_t getDescriptorCountForType(const std::tuple<T, Rest...>& parameters) {
  if (std::get<Iterator>(parameters).getDescriptorType() == DescriptorType) {
    if constexpr (Iterator < sizeof...(Rest))
      return std::get<Iterator>(parameters).getDescriptorCount() +
             getDescriptorCountForType<Iterator + 1, DescriptorType, T,
                                       Rest...>(parameters);
    else
      return std::get<Iterator>(parameters).getDescriptorCount();
  } else {
    if constexpr (Iterator < sizeof...(Rest))
      return getDescriptorCountForType<Iterator + 1, DescriptorType, T,
                                       Rest...>(parameters);
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
      getDescriptorCountForType<0, vk::DescriptorType::eUniformBuffer,
                                TShaderParameters...>(mParameters);
  if (uniformBufferCount > 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize()
                               .setDescriptorCount(uniformBufferCount)
                               .setType(vk::DescriptorType::eUniformBuffer));
  }
  uint32_t combinedSamplerCount =
      getDescriptorCountForType<0, vk::DescriptorType::eCombinedImageSampler,
                                TShaderParameters...>(mParameters);
  if (combinedSamplerCount > 0) {
    poolSizes.emplace_back(
        vk::DescriptorPoolSize()
            .setDescriptorCount(combinedSamplerCount)
            .setType(vk::DescriptorType::eCombinedImageSampler));
  }

  uint32_t storageImageCount =
      getDescriptorCountForType<0, vk::DescriptorType::eStorageImage,
                                TShaderParameters...>(mParameters);
  if (storageImageCount > 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize()
                               .setDescriptorCount(storageImageCount)
                               .setType(vk::DescriptorType::eStorageImage));
  }

  vk::DescriptorPoolCreateInfo poolCreateInfo =
      vk::DescriptorPoolCreateInfo().setPoolSizes(poolSizes).setMaxSets(
          sShaderParameterCount);
  mDescriptorPool = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createDescriptorPool(poolCreateInfo));
  std::vector<vk::DescriptorSetLayoutBinding> bindingInfos(paramCount);
  initializeBindings<0, TShaderParameters...>(mParameters, bindingInfos, stage);

  vk::DescriptorSetLayoutCreateInfo layoutCreateInfo =
      vk::DescriptorSetLayoutCreateInfo()
          .setBindingCount(paramCount)
          .setBindings(bindingInfos);

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

  bindAllParametersToGroup(mParameters, mDescriptorSet);
}

template <class... TShaderParameters>
ShaderParameterGroup<TShaderParameters...>::ShaderParameterGroup(
    ShaderParameterGroup&& other) noexcept
    : mParameters(std::move(other.mParameters)),
      mDescriptorPool(std::move(other.mDescriptorPool)),
      mDescriptorSet(std::move(other.mDescriptorSet)),
      mLayout(std::move(other.mLayout)) {
  other.mDescriptorPool = nullptr;
  other.mDescriptorSet = nullptr;
  other.mLayout = nullptr;
}

template <class... TShaderParameters>
ShaderParameterGroup<TShaderParameters...>::~ShaderParameterGroup() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyDescriptorSetLayout(mLayout);
  device.getLogicalDeviceHandle().destroyDescriptorPool(mDescriptorPool);
}
