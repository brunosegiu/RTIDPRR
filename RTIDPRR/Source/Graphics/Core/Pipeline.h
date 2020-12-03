#pragma once

#include <vulkan/vulkan.hpp>

#include "../Geometry/GeometryLayout.h"
#include "RenderPass.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderParameterGroup.h"

namespace RTIDPRR {
namespace Graphics {
class Pipeline {
 public:
  Pipeline(const RenderPass& renderPass, const vk::Extent2D& extent,
           const GeometryLayout& geometryLayout,
           const std::vector<std::string>& shaderPaths,
           const std::vector<vk::DescriptorSetLayout>& descriptorLayouts,
           const std::vector<vk::PushConstantRange>& pushConstants);

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
