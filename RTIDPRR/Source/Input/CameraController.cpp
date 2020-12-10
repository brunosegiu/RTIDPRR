#include "CameraController.h"

#include "../Misc/Window.h"

using namespace RTIDPRR::Input;

CameraController::CameraController(RTIDPRR::Core::Camera& camera)
    : InputEventListener(),
      mCamera(camera),
      mSpeed(0.00001f),
      mIsEnabled(false) {}

void CameraController::onKeyPress(const float timeDelta,
                                  const SDL_Scancode key) {
  if (isEnabled()) {
    const float factor = mSpeed * timeDelta;
    switch (key) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_UP:
        mCamera.moveForwards(factor);
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        mCamera.moveLeft(factor);
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        mCamera.moveBackwards(factor);
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        mCamera.moveRight(factor);
        break;
      default:
        break;
    }
  }
}

void CameraController::onCursorMovement(const float timeDelta,
                                        const glm::vec2& position) {
  if (isEnabled()) {
    mCamera.rotate(position.x, position.y);
  }
}
void CameraController::onRightClick(const float timeDelta) {
  mIsEnabled = true;
}

void CameraController::onRightClickReleased(const float timeDelta) {
  mIsEnabled = false;
}

void CameraController::onInputProcessed() {
  RTIDPRR::Window& window = RTIDPRR::Window::get();
  if (mIsEnabled) {
    window.warpCursorToCenter();
  }
  window.toggleCursor(!isEnabled());
}

CameraController::~CameraController() {}

bool CameraController::isEnabled() { return mIsEnabled; }
