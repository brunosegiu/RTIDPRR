#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../Core/Context.h"
#include "../Core/Pipeline.h"
#include "../Core/Shaders/ShaderParameterGroup.h"
#include "../Geometry/IndexedVertexBuffer.h"

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

  IndexedVertexBuffer mMesh;
};
}  // namespace Graphics
}  // namespace RTIDPRR
