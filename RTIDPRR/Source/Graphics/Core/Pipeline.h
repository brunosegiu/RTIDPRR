#pragma once

#include <vulkan/vulkan.hpp>

#include "../Geometry/GeometryLayout.h"
#include "RenderPass.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderParameterConstantGroup.h"
#include "Shaders/ShaderParameterGroup.h"

namespace RTIDPRR {
namespace Graphics {
struct PipelineCreateOptions {
  vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack;
  bool enableDepthBias = false;

  PipelineCreateOptions& setCullMode(vk::CullModeFlagBits mode) {
    cullMode = mode;
    return *this;
  }

  PipelineCreateOptions& setEnableDepthBias(bool enable) {
    enableDepthBias = enable;
    return *this;
  }
};

class Pipeline {
 public:
  Pipeline(const RenderPass& renderPass, const vk::Extent2D& extent,
           const GeometryLayout& geometryLayout,
           const std::vector<std::string>& shaderPaths,
           const std::vector<vk::DescriptorSetLayout>& descriptorLayouts,
           const std::vector<vk::PushConstantRange>& pushConstants,
           const SpecializationMap& specializationConstants,
           const PipelineCreateOptions& options = PipelineCreateOptions());

  Pipeline(Pipeline&& other)
      : mPipelineHandle(std::move(other.mPipelineHandle)),
        mLayoutHandle(std::move(other.mLayoutHandle)),
        mShaders(std::move(other.mShaders)) {
    other.mPipelineHandle = nullptr;
    other.mLayoutHandle = nullptr;
  }

  const vk::Pipeline& getPipelineHandle() const { return mPipelineHandle; }
  const vk::PipelineLayout& getPipelineLayout() const { return mLayoutHandle; }

  virtual ~Pipeline();

 private:
  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mLayoutHandle;

  std::vector<Shader*> mShaders;
};
}  // namespace Graphics
}  // namespace RTIDPRR
