#pragma once

#include "../Components/Camera/Camera.h"
#include "../Components/Camera/CameraSystem.h"
#include "../Components/Light/LightSystem.h"
#include "../Components/Mesh/Mesh.h"
#include "../Components/Transform/TransformSystem.h"
#include "Object.h"
#include "System.h"

namespace RTIDPRR {
namespace Core {
class Scene {
 public:
  Scene();

  const RTIDPRR::Component::Camera& getCamera() const { return *mCamera; }
  RTIDPRR::Component::Camera& getCamera() { return *mCamera; }

  template <class TSystem>
  TSystem& getSystem();

  Object& addObject();

  void update(float deltaTime);

  virtual ~Scene();

 private:
  RTIDPRR::Component::Camera* mCamera;

  std::list<Object> mObjects;

  RTIDPRR::Component::TransformSystem mTransformSystem;
  RTIDPRR::Core::System<RTIDPRR::Component::Mesh> mMeshSystem;
  RTIDPRR::Component::CameraSystem mCameraSystem;
  RTIDPRR::Component::LightSystem mLightSystem;

  void updateSystems(float deltaTime);
};
}  // namespace Core
}  // namespace RTIDPRR
