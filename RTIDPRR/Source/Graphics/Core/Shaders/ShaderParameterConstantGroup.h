#pragma once

#include <tuple>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "../Context.h"
#include "ShaderParameter.h"

namespace RTIDPRR {
namespace Graphics {
struct SpecializationInfo {
  std::vector<vk::SpecializationMapEntry> entries;
  const void* data;
  size_t size;
};

using SpecializationMap =
    std::unordered_map<vk::ShaderStageFlagBits, SpecializationInfo>;

template <class... TConstantGroups>
SpecializationMap generateSpecializationMap(
    const TConstantGroups&... constants) {
  SpecializationMap map;
  generateSpecializationMapAux(map, constants...);
  return map;
}

template <class TConstantGroup, class... TRest>
void generateSpecializationMapAux(SpecializationMap& map,
                                  const TConstantGroup& group,
                                  TRest... others) {
  map.emplace(group.getShaderStage(), group.getSpecializationInfo());
  if constexpr (sizeof...(TRest) > 0) generateSpecializationMap(map, others...);
}

template <class... TParameter>
class ShaderParameterConstantGroup {
 public:
  ShaderParameterConstantGroup(const vk::ShaderStageFlagBits& stage,
                               TParameter... values);

  static constexpr uint32_t sShaderParameterCount =
      std::tuple_size<std::tuple<TParameter...>>::value;

  SpecializationInfo getSpecializationInfo() const;
  vk::ShaderStageFlagBits getShaderStage() const { return mStage; }

  virtual ~ShaderParameterConstantGroup();

 private:
  template <size_t Iterator, size_t Offset, class TParam, class... Rest>
  void initEntries();

  std::vector<vk::SpecializationMapEntry> mEntries;
  std::tuple<TParameter...> mValues;
  vk::ShaderStageFlagBits mStage;
};
}  // namespace Graphics
}  // namespace RTIDPRR

using namespace RTIDPRR::Graphics;

template <class... TParameter>
template <size_t Iterator, size_t Offset, class TParam, class... Rest>
void ShaderParameterConstantGroup<TParameter...>::initEntries() {
  mEntries[Iterator]
      .setOffset(Offset)
      .setSize(sizeof(TParam))
      .setConstantID(Iterator);
  if constexpr (sizeof...(Rest) > 0)
    initEntries<Iterator + 1, Offset + sizeof(TParam), Rest...>();
}

template <class... TParameter>
ShaderParameterConstantGroup<TParameter...>::ShaderParameterConstantGroup(
    const vk::ShaderStageFlagBits& stage, TParameter... values)
    : mEntries(sShaderParameterCount), mStage(stage), mValues(values...) {
  initEntries<0, 0, TParameter...>();
}

template <class... TParameter>
inline SpecializationInfo
ShaderParameterConstantGroup<TParameter...>::getSpecializationInfo() const {
  SpecializationInfo specializationInfo;
  specializationInfo.entries = mEntries;
  specializationInfo.data = static_cast<const void*>(&mValues);
  specializationInfo.size = sizeof(mValues);
  return specializationInfo;
}

template <class... TParameter>
ShaderParameterConstantGroup<TParameter...>::~ShaderParameterConstantGroup() {}
