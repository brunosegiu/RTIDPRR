#pragma once

#include "../../Core/System.h"
#include "Light.h"

namespace RTIDPRR {
namespace Component {
class LightSystem : public RTIDPRR::Core::System<Light> {
 public:
  LightSystem();

  virtual void update(const float deltaTime) override;

  virtual ~LightSystem();
};

}  // namespace Component
}  // namespace RTIDPRR
