#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../../Core/Component.h"
#include "../../Core/System.h"
#include "../../Input/InputEventListener.h"
#include "../Transform/Transform.h"

namespace RTIDPRR {
namespace Component {
class CameraSystem;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Component {
class Camera : public RTIDPRR::Core::Component,
               public RTIDPRR::Input::InputEventListener {
 public:
  using SystemType = CameraSystem;

  Camera(RTIDPRR::Core::Object* object, float nearPlane = 0.01f,
         float farPlane = 1000.0f, float fov = 45.0f);
  Camera(Camera&& other) noexcept;

  const glm::mat4& getViewProjection() { return mViewProjection; }

  void update();

  // Event listeners
  void onKeyPress(const float timeDelta, const SDL_Scancode key) override;
  void onCursorMovement(const float timeDelta,
                        const glm::vec2& position) override;
  void onRightClick(const float timeDelta) override;
  void onRightClickReleased(const float timeDelta) override;

  void onInputProcessed() override;

  ~Camera();

 private:
  glm::vec3 mUp;

  float mNear, mFar, mFOV;

  glm::mat4 mProjection;
  glm::mat4 mView;
  glm::mat4 mViewProjection;

  Transform* mTransform;

  float mSpeed;
  bool mIsEnabled;

  glm::vec3 getDirection();

  bool isEnabled() { return mIsEnabled; }
};

}  // namespace Component
}  // namespace RTIDPRR
