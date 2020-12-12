#include "CameraSystem.h"

using namespace RTIDPRR::Component;

CameraSystem::CameraSystem() : RTIDPRR::Core::System<Camera>() {}

void CameraSystem::update(const float deltaTime) {
  // Collect parents
  std::vector<Transform*> roots;
  for (Camera& camera : mComponents) {
    camera.update();
  }
}

CameraSystem::~CameraSystem() {}
