#pragma once

#include <glm/glm.hpp>

#include "../../Core/Component.h"
#include "../../Core/System.h"

namespace RTIDPRR {
namespace Component {
struct LightProxy {
  glm::vec4 mPosition;
  glm::vec4 mDirection;
  float mIntensity;
};

class Light : public RTIDPRR::Core::Component {
 public:
  using SystemType = RTIDPRR::Core::System<Light>;

  Light(RTIDPRR::Core::Object* object);
  Light(Light&& other) noexcept;

  LightProxy getProxy() const;

  ~Light();

 private:
  float mIntensity;
};

}  // namespace Component
}  // namespace RTIDPRR
