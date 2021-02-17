#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Command.h"
#include "../Core/Pipeline.h"
#include "../Core/Shaders/ShaderParameterInlineGroup.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../Core/Shaders/ShaderParameterTextureArray.h"
#include "../Geometry/IndexedVertexBuffer.h"

namespace RTIDPRR {
namespace Graphics {

struct CameraMatrices {
  glm::mat4 mModel;
  glm::mat4 mvp;
};

class GizmoRenderer {
 public:
  GizmoRenderer();

  void render(Scene& scene);

  void renderBox(const glm::mat4& viewProjectionMatrix,
                 const glm::mat4& modelMatrix);

  virtual ~GizmoRenderer();

 private:
  Command* mCommand;
  ShaderParameterInlineGroup<CameraMatrices> mInlineParameters;
  Pipeline mLinePipeline;

  IndexedVertexBuffer mBoxGeometry;
};

}  // namespace Graphics
}  // namespace RTIDPRR
