#include "Scene.h"

#include "../Misc/DebugUtils.h"

using namespace RTIDPRR::Core;

Scene::Scene()
    : mTransformSystem(), mMeshSystem(), mCameraSystem(), mLightSystem() {
  Object& camera = addObject();
  mCamera = camera.addComponent<RTIDPRR::Component::Camera>();
  camera.getComponent<RTIDPRR::Component::Transform>()->setLocalTranslation(
      glm::vec3(-5.0f, 0, 0.0f));
}

Object& Scene::addObject() { return mObjects.emplace_back(this); }

void Scene::update(float deltaTime) { updateSystems(deltaTime); }

using namespace RTIDPRR::Core;

template <class TSystem>
TSystem& Scene::getSystem() {
  RTIDPRR_ASSERT_MSG(false, "No specialization found for this System!");
  void* nullValue = (void*)0;
  return *reinterpret_cast<TSystem*>(nullValue);
}

template <>
RTIDPRR::Component::TransformSystem& Scene::getSystem() {
  return mTransformSystem;
}

template <>
RTIDPRR::Core::System<RTIDPRR::Component::Mesh>& Scene::getSystem() {
  return mMeshSystem;
}

template <>
RTIDPRR::Component::CameraSystem& Scene::getSystem() {
  return mCameraSystem;
}

template <>
RTIDPRR::Core::System<RTIDPRR::Component::Light>& Scene::getSystem() {
  return mLightSystem;
}

void Scene::updateSystems(float deltaTime) {
  mTransformSystem.update(deltaTime);
  mCameraSystem.update(deltaTime);
}

Scene::~Scene() {}
