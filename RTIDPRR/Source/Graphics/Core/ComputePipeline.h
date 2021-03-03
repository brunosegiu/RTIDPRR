#pragma once

#include <vulkan/vulkan.hpp>

#include "Shaders/Shader.h"
#include "Shaders/ShaderParameterConstantGroup.h"
#include "Shaders/ShaderParameterGroup.h"

namespace RTIDPRR {
namespace Graphics {

class ComputePipeline {
 public:
  ComputePipeline(
      const std::string& shaderPath,
      const std::vector<vk::DescriptorSetLayout>& descriptorLayouts = {},
      const std::vector<vk::PushConstantRange>& pushConstants = {},
      const SpecializationMap& specializationConstants = {});

  ComputePipeline(ComputePipeline&& other) noexcept
      : mPipelineHandle(std::move(other.mPipelineHandle)),
        mLayoutHandle(std::move(other.mLayoutHandle)),
        mShader(std::move(other.mShader)) {
    other.mPipelineHandle = nullptr;
    other.mLayoutHandle = nullptr;
    other.mShader = nullptr;
  }

  const vk::Pipeline& getPipelineHandle() const { return mPipelineHandle; }
  const vk::PipelineLayout& getPipelineLayout() const { return mLayoutHandle; }

  virtual ~ComputePipeline();

 private:
  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mLayoutHandle;

  Shader* mShader;
};
}  // namespace Graphics
}  // namespace RTIDPRR
