﻿#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../../Core/Object.h"
#include "../../Misc/Window.h"

using namespace RTIDPRR::Component;

Camera::Camera(RTIDPRR::Core::Object* object, float nearPlane, float farPlane,
               float fov)
    : Component(object),
      InputEventListener(),
      mProjection(1.0f),
      mView(1.0f),
      mViewProjection(1.0f),
      mUp(0.0f, 1.0f, 0.0f),
      mNear(nearPlane),
      mFar(farPlane),
      mFOV(fov),
      mIsEnabled(false),
      mSpeed(0.00001f),
      mFrustum(mViewProjection) {
  mProjection = glm::perspective(glm::radians(mFOV), 1.0f, mNear, mFar);
  // Account for Vulkan flipped y
  mProjection[1][1] *= -1.0f;
  mUp = glm::vec3(0.0f, 1.0f, 0.0f);
  mTransform = object->getComponent<Transform>();
  Window::get().getInputController().attach(this);
}

Camera::Camera(Camera&& other) noexcept
    : Component(std::move(other)),
      InputEventListener(std::move(other)),
      mProjection(std::move(other.mProjection)),
      mView(std::move(other.mView)),
      mViewProjection(std::move(other.mViewProjection)),
      mUp(std::move(other.mUp)),
      mNear(std::move(other.mNear)),
      mFar(std::move(other.mFar)),
      mFOV(std::move(other.mFOV)),
      mIsEnabled(std::move(other.mIsEnabled)),
      mSpeed(std::move(other.mSpeed)),
      mFrustum(std::move(other.mFrustum)) {}

void Camera::update() {
  mProjection = glm::perspective(glm::radians(mFOV), 16.0f / 9.0f, mNear, mFar);
  mProjection[1][1] *= -1.0f;
  glm::vec3 eyePos = mTransform->getAbsoluteTranslation();
  mView = glm::lookAt(eyePos, eyePos + mTransform->getDirection(), mUp);
  mViewProjection = mProjection * mView;
  mFrustum = Frustum(mViewProjection);
}

void Camera::onKeyPress(const float timeDelta, const SDL_Scancode key) {
  if (isEnabled()) {
    glm::vec3 currentDir = mTransform->getDirection();
    const float factor = mSpeed * timeDelta;
    switch (key) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_UP:
        mTransform->translate(currentDir * factor);
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        mTransform->translate(glm::cross(mUp, currentDir) * factor);
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        mTransform->translate(-currentDir * factor);
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        mTransform->translate(-glm::cross(mUp, currentDir) * factor);
        break;
      default:
        break;
    }
  }
}

void Camera::onCursorMovement(const float timeDelta,
                              const glm::vec2& position) {
  if (isEnabled()) {
    glm::vec3 eulerRotation = glm::vec3(0.0f, position.x, position.y);
    mTransform->rotate(glm::quat(eulerRotation));
  }
}
void Camera::onRightClick(const float timeDelta) { mIsEnabled = true; }

void Camera::onRightClickReleased(const float timeDelta) { mIsEnabled = false; }

void Camera::onInputProcessed() {
  RTIDPRR::Window& window = RTIDPRR::Window::get();
  if (mIsEnabled) {
    window.warpCursorToCenter();
  }
  window.toggleCursor(!isEnabled());
}

Camera::~Camera() {}
