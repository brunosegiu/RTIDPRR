#include "Light.h"

#include "../../Core/Object.h"
#include "../../Graphics/GizmoRendering/GizmoRenderer.h"
#include "../Transform/Transform.h"

using namespace RTIDPRR::Component;

static const float frustumWidth = 30.0f;
static const float lightFar = 100.0f;
static const float lightNear = 0.5f;

Light::Light(RTIDPRR::Core::Object* object)
    : Component(object), mIntensity(1.0f), mFrustum(glm::mat4(1.0f)) {}

void Light::renderGizmos(GizmoRenderer* renderer) const {
  const Transform* transform = getObject()->getComponent<Transform>();
  Scene* scene = getObject()->getScene();

  glm::mat4 modelMat =
      glm::translate(glm::mat4(1.0f), transform->getAbsoluteTranslation() +
                                          glm::vec3(lightFar, 0.0f, 0.0f));
  modelMat *= glm::mat4_cast(transform->getAbsoluteRotation());
  modelMat =
      glm::scale(modelMat, glm::vec3(lightFar, frustumWidth, frustumWidth));

  renderer->renderBox(scene->getCamera().getViewProjection(), modelMat);
  renderer->renderBox(
      scene->getCamera().getViewProjection(),
      glm::translate(glm::mat4(1.0f), transform->getAbsoluteTranslation()));
}

Light::Light(Light&& other) noexcept
    : Component(std::move(other)),
      mProxy(std::move(other.mProxy)),
      mFrustum(std::move(other.mFrustum)) {}

void Light::update() {
  Transform* transform = getObject()->getComponent<Transform>();

  glm::vec3 lightDir = transform->getDirection();
  glm::mat4 projection =
      glm::ortho<float>(-frustumWidth, frustumWidth, -frustumWidth,
                        frustumWidth, lightNear, lightFar);
  glm::mat4 view = glm::lookAt(transform->getAbsoluteTranslation(),
                               transform->getAbsoluteTranslation() + lightDir,
                               glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 lightMatrix = projection * view;
  mProxy = {lightMatrix, glm::vec4(lightDir, 1.0f), mIntensity};
  mFrustum.update(lightMatrix);
}

Light::~Light() {}
