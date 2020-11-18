#pragma once

#include <vulkan/vulkan.hpp>

#include "Shader.h"

namespace RTIDPRR {
namespace Graphics {
class Pipeline {
 public:
  Pipeline(const vk::RenderPass& renderPass, const Shader& vertexShader,
           const Shader& fragmentShader);

  const vk::Pipeline& getPipelineHandle() const { return mPipelineHandle; }

  virtual ~Pipeline();

 private:
  vk::Pipeline mPipelineHandle;
  vk::PipelineLayout mLayoutHandle;
};
}  // namespace Graphics
}  // namespace RTIDPRR
