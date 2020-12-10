#include "Component.h"

using namespace RTIDPRR::Core;

Component::Component(Object* object) : mPObject(object) {}

Component::Component(Component&& other) noexcept : mPObject(other.mPObject) {
  mPObject = nullptr;
}

Component::~Component() {}
