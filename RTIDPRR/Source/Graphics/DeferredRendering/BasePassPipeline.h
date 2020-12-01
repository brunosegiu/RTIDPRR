#pragma once

#include <vulkan/vulkan.hpp>

#include "../Core/Shaders/Shader.h"

namespace RTIDPRR {
namespace Graphics {
class BasePassPipeline {
 public:
  BasePassPipeline(const vk::RenderPass& renderPass,
                   const std::vector<vk::DescriptorSetLayout> descriptorLayouts,
                   const vk::Extent2D extent);

  const vk::Pipeline& getPipelineHandle() const { return mPipelineHandle; }
  const vk::PipelineLayout& getPipelineLayout() const { return mLayoutHandle; }

  virtual ~BasePassPipeline();

 private:
  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mLayoutHandle;
  Shader *mVertexShader, *mFragmentShader;
};
}  // namespace Graphics
}  // namespace RTIDPRR
