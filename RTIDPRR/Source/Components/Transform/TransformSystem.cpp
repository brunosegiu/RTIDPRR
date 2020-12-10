#include "TransformSystem.h"

#include "../../Core/Scene.h"

using namespace RTIDPRR::Component;

TransformSystem::TransformSystem() : RTIDPRR::Core::System<Transform>() {}

void TransformSystem::update(const float deltaTime) {
  // Collect parents
  std::vector<Transform*> roots;
  for (Transform& transform : mComponents) {
    if (transform.getParent() == nullptr) {
      roots.push_back(&transform);
    }
  }
  for (Transform* root : roots) {
    root->update(glm::mat4(1.0f));
  }
}

TransformSystem::~TransformSystem() {}
