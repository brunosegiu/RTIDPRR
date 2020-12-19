#include "LightSystem.h"

#include "../../Core/Scene.h"

using namespace RTIDPRR::Component;

LightSystem::LightSystem() : RTIDPRR::Core::System<Light>() {}

void LightSystem::update(const float deltaTime) {
  for (Light& light : mComponents) {
    light.update();
  }
}

LightSystem::~LightSystem() {}
