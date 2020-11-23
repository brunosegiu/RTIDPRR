#pragma once

#include <vulkan/vulkan.hpp>

#include "Shaders/Shader.h"

namespace RTIDPRR {
namespace Graphics {
class Pipeline {
 public:
  Pipeline(const vk::RenderPass& renderPass, const Shader& vertexShader,
           const Shader& fragmentShader,
           const std::vector<vk::DescriptorSetLayout>& descriptorLayouts);

  const vk::Pipeline& getPipelineHandle() const { return mPipelineHandle; }
  const vk::PipelineLayout& getPipelineLayout() const { return mLayoutHandle; }

  virtual ~Pipeline();

 private:
  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mLayoutHandle;
};
}  // namespace Graphics
}  // namespace RTIDPRR
