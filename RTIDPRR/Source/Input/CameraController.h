﻿#pragma once

#include "../Graphics/Scene/Camera.h"
#include "InputEventListener.h"

namespace RTIDPRR {
namespace Input {
class CameraController : public InputEventListener {
 public:
  CameraController(RTIDPRR::Graphics::Camera& camera);

  void onKeyPress(const float timeDelta, const SDL_Scancode key) override;
  void onCursorMovement(const float timeDelta,
                        const glm::vec2& position) override;
  void onRightClick(const float timeDelta) override;
  void onRightClickReleased(const float timeDelta) override;

  void onInputProcessed() override;

  virtual ~CameraController();

 private:
  RTIDPRR::Graphics::Camera& mCamera;
  float mSpeed;
  bool mIsEnabled;

  bool isEnabled();
};
}  // namespace Input
}  // namespace RTIDPRR