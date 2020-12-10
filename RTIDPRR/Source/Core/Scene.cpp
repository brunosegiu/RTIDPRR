#include "Scene.h"

using namespace RTIDPRR::Core;

Scene::Scene() : mCamera(), mTransformSystem(), mMeshSystem() {}

Object& Scene::addObject() { return mObjects.emplace_back(this); }

void Scene::update(float deltaTime) { updateSystems(deltaTime); }

using namespace RTIDPRR::Core;

template <class TSystem>
TSystem& Scene::getSystem() {
  RTIDPRR_ASSERT_MSG(false, "No specialization found for this System!");
  return *(nullptr)
}

template <>
RTIDPRR::Component::TransformSystem& Scene::getSystem() {
  return mTransformSystem;
}

template <>
RTIDPRR::Component::MeshSystem& Scene::getSystem() {
  return mMeshSystem;
}

void Scene::updateSystems(float deltaTime) {
  mMeshSystem.update(deltaTime);
  mTransformSystem.update(deltaTime);
}

Scene::~Scene() {}
