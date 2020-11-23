#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../Core/Context.h"
#include "../Core/Pipeline.h"
#include "../Core/Shaders/ShaderParameterGroup.h"

namespace RTIDPRR {
namespace Graphics {
class DeferredRenderer {
 public:
  DeferredRenderer();

  void render();

  virtual ~DeferredRenderer();

 private:
  ShaderParameterGroup<ShaderParameter<glm::vec4>> mShaderParameters;

  Pipeline mPipeline;
};
}  // namespace Graphics
}  // namespace RTIDPRR
