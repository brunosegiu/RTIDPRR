#pragma once

#include <glm/glm.hpp>

#include "../../Core/Component.h"
#include "../../Core/System.h"
#include "../../Misc/Frustum.h"

namespace RTIDPRR {
namespace Component {
class LightSystem;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Component {
struct LightProxy {
  glm::mat4 viewProjection;
  glm::vec4 direction = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  float intensity = 0.0f;
};

class Light : public RTIDPRR::Core::Component {
 public:
  using SystemType = LightSystem;

  Light(RTIDPRR::Core::Object* object);
  Light(Light&& other) noexcept;

  float getIntensity() const { return mIntensity; }
  void setIntensity(float intensity) { mIntensity = intensity; }

  virtual void renderGizmos(
      RTIDPRR::Graphics::GizmoRenderer* renderer) const override;

  void update();

  LightProxy getProxy() const { return mProxy; }
  Frustum getFrustum() const { return mFrustum; }

  ~Light();

 private:
  float mIntensity;

  LightProxy mProxy;
  Frustum mFrustum;
};

}  // namespace Component
}  // namespace RTIDPRR
