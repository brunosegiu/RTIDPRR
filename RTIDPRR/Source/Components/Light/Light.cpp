#include "Light.h"

#include "../../Core/Object.h"
#include "../Transform/Transform.h"

using namespace RTIDPRR::Component;

Light::Light(RTIDPRR::Core::Object* object)
    : Component(object), mIntensity(1.0f) {}

Light::Light(Light&& other) noexcept : Component(std::move(other)) {}

LightProxy Light::getProxy() const {
  Transform* transform = getObject()->getComponent<Transform>();
  return {glm::vec4(transform->getAbsoluteTranslation(), 1.0f),
          glm::vec4(transform->getDirection(), 1.0f), mIntensity};
}

Light::~Light() {}
