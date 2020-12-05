#pragma once

#include <tuple>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "../Context.h"
#include "ShaderParameter.h"

namespace RTIDPRR {
namespace Graphics {
template <class... TParameter>
class ShaderParameterInlineGroup {
 public:
  ShaderParameterInlineGroup(const std::vector<vk::ShaderStageFlags>& stages);

  static constexpr uint32_t sShaderParameterCount =
      std::tuple_size<std::tuple<TParameter...>>::value;

  const std::vector<vk::PushConstantRange>& getPushConstantRanges() const {
    return mPushConstantRanges;
  }

  virtual ~ShaderParameterInlineGroup();

 private:
  template <size_t Iterator, size_t Offset, class TParam, class... Rest>
  void initPushConstantRanges(const std::vector<vk::ShaderStageFlags>& stages);

  std::vector<vk::PushConstantRange> mPushConstantRanges;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <class... TParameter>
template <size_t Iterator, size_t Offset, class TParam, class... Rest>
void ShaderParameterInlineGroup<TParameter...>::initPushConstantRanges(
    const std::vector<vk::ShaderStageFlags>& stages) {
  mPushConstantRanges[Iterator]
      .setOffset(Offset)
      .setSize(sizeof(TParam))
      .setStageFlags(stages[Iterator]);
  if constexpr (Iterator < sizeof...(Rest))
    initPushConstantRanges<Iterator + 1, Offset + sizeof(TParam)>(stages);
}

template <class... TParameter>
ShaderParameterInlineGroup<TParameter...>::ShaderParameterInlineGroup(
    const std::vector<vk::ShaderStageFlags>& stages)
    : mPushConstantRanges(sShaderParameterCount) {
  initPushConstantRanges<0, 0, TParameter...>(stages);
}

template <class... TParameter>
ShaderParameterInlineGroup<TParameter...>::~ShaderParameterInlineGroup() {}
