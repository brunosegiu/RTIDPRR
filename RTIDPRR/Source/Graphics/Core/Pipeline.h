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
  float depthBiasConstant = 0.0f;
  float depthBiasSlope = 0.0f;
  vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;

  PipelineCreateOptions& setCullMode(vk::CullModeFlagBits mode) {
    cullMode = mode;
    return *this;
  }

  PipelineCreateOptions& setEnableDepthBias(bool enable) {
    enableDepthBias = enable;
    return *this;
  }

  PipelineCreateOptions& setDepthBiasConstant(float factor) {
    depthBiasConstant = factor;
    return *this;
  }

  PipelineCreateOptions& setDepthBiasSlope(float factor) {
    depthBiasSlope = factor;
    return *this;
  }

  PipelineCreateOptions& setTopology(vk::PrimitiveTopology topology) {
    this->topology = topology;
    return *this;
  }
};

class Pipeline {
 public:
  Pipeline(const RenderPass& renderPass, const vk::Extent2D& extent,
           const GeometryLayout& geometryLayout,
           const std::vector<std::string>& shaderPaths,
           const std::vector<vk::DescriptorSetLayout>& descriptorLayouts = {},
           const std::vector<vk::PushConstantRange>& pushConstants = {},
           const SpecializationMap& specializationConstants = {},
           const PipelineCreateOptions& options = PipelineCreateOptions());

  Pipeline(Pipeline&& other) noexcept
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
