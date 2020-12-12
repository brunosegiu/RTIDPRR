#pragma once

#include "../../Core/System.h"
#include "Camera.h"

namespace RTIDPRR {
namespace Component {
class CameraSystem : public RTIDPRR::Core::System<Camera> {
 public:
  CameraSystem();

  virtual void update(const float deltaTime) override;

  virtual ~CameraSystem();
};

}  // namespace Component
}  // namespace RTIDPRR
