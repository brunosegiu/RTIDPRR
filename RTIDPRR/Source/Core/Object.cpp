#include "Object.h"

#include "../Components/Transform/Transform.h"
#include "../Misc/Random/UUIDGenerator.h"

using namespace RTIDPRR::Core;

Object::Object(Scene* scene)
    : mScene(scene),
      mId(RTIDPRR::Random::UUIDGenerator::getId()),
      mComponents() {
  addComponent<RTIDPRR::Component::Transform>();
}

Object::Object(Object&& other) noexcept : mScene(other.mScene), mId(other.mId) {
  other.mScene = nullptr;
}

void Object::renderGizmos(RTIDPRR::Graphics::GizmoRenderer* renderer) const {
  for (Component* component : mComponents) {
    component->renderGizmos(renderer);
  }
}

Object::~Object() {}
