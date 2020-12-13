#pragma once

#include <glm/glm.hpp>

#include "../../Core/Component.h"
#include "../../Core/System.h"

namespace RTIDPRR {
namespace Component {
struct LightProxy {
  alignas(16) glm::vec4 mDirection = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  alignas(4) float mIntensity = 0.0f;
};

class Light : public RTIDPRR::Core::Component {
 public:
  using SystemType = RTIDPRR::Core::System<Light>;

  Light(RTIDPRR::Core::Object* object);
  Light(Light&& other) noexcept;

  float getIntensity() const { return mIntensity; }
  void setIntensity(float intensity) { mIntensity = intensity; }

  LightProxy getProxy() const;

  ~Light();

 private:
  float mIntensity;
};

}  // namespace Component
}  // namespace RTIDPRR
