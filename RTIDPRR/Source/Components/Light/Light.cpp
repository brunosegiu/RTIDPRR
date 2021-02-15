#include "Light.h"

#include "../../Core/Object.h"
#include "../Transform/Transform.h"

using namespace RTIDPRR::Component;

Light::Light(RTIDPRR::Core::Object* object)
    : Component(object), mIntensity(1.0f), mFrustum(glm::mat4(1.0f)) {}

Light::Light(Light&& other) noexcept
    : Component(std::move(other)),
      mProxy(std::move(other.mProxy)),
      mFrustum(std::move(other.mFrustum)) {}

void Light::update() {
  Transform* transform = getObject()->getComponent<Transform>();

  glm::vec3 lightDir = transform->getDirection();
  const float width = 30.0f;
  glm::mat4 projection =
      glm::ortho<float>(-width, width, -width, width, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(transform->getAbsoluteTranslation(),
                               transform->getAbsoluteTranslation() + lightDir,
                               glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 lightMatrix = projection * view;
  mProxy = {lightMatrix, glm::vec4(lightDir, 1.0f), mIntensity};
  mFrustum.update(lightMatrix);
}

Light::~Light() {}
