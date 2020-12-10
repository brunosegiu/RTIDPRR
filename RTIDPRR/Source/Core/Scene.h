#pragma once

#include "../Components/Mesh/MeshSystem.h"
#include "../Components/Transform/TransformSystem.h"
#include "Camera.h"
#include "Object.h"
#include "System.h"

namespace RTIDPRR {
namespace Core {
class Scene {
 public:
  Scene();

  const Camera& getCamera() const { return mCamera; }
  Camera& getCamera() { return mCamera; }

  template <class TSystem>
  TSystem& getSystem();

  Object& addObject();

  void update(float deltaTime);

  virtual ~Scene();

 private:
  Camera mCamera;

  std::list<Object> mObjects;

  RTIDPRR::Component::TransformSystem mTransformSystem;
  RTIDPRR::Component::MeshSystem mMeshSystem;

  void updateSystems(float deltaTime);
};
}  // namespace Core
}  // namespace RTIDPRR
