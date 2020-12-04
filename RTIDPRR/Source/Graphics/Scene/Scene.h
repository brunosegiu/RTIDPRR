#pragma once

#include "../Geometry/IndexedVertexBuffer.h"
#include "Camera.h"

namespace RTIDPRR {
namespace Graphics {
class Scene {
 public:
  Scene();

  const std::vector<IndexedVertexBuffer>& getMeshes() const { return mMeshes; }

  const Camera& getCamera() const { return mCamera; }
  Camera& getCamera() { return mCamera; }

  virtual ~Scene();

 private:
  std::vector<IndexedVertexBuffer> mMeshes;
  Camera mCamera;
};
}  // namespace Graphics
}  // namespace RTIDPRR
