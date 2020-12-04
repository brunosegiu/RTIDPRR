#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace RTIDPRR::Graphics;

Camera::Camera() {
  mProjection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);
  // Account for Vulkan flipped y
  mProjection[1][1] *= -1.0f;

  mEye = glm::vec3(-5, 0, 5.0f);
  mDirection = glm::normalize(glm::vec3(5, 0, -5.0f));
  mUp = glm::vec3(0, 1, 0);

  updateViewProjection();
}

void Camera::updateViewProjection() const {
  mView = glm::lookAt(mEye, mEye + mDirection * glm::length(mEye), mUp);
  mVP = mProjection * mView;
}

void Camera::rotate(const float& angle, const glm::vec3& axis) {
  mDirection = glm::mat3(glm::rotate(angle, axis)) * mDirection;
}

void Camera::translate(const glm::vec3& value) { mEye += value; }

void Camera::translateTo(const glm::vec3& value) { mEye = value; }

void Camera::moveRight(const float& value) {
  const glm::vec3 right = glm::cross(mDirection, mUp);
  translate(right * value);
}

void Camera::moveLeft(const float& value) {
  const glm::vec3 left = -glm::cross(mDirection, mUp);
  translate(left * value);
}

void Camera::moveForwards(const float& value) { translate(mDirection * value); }

void Camera::moveBackwards(const float& value) {
  translate(-mDirection * value);
}

void Camera::rotate(const float& yaw, const float& pitch) {
  const glm::vec3 right = glm::cross(mDirection, mUp);
  rotate(yaw, mUp);
  rotate(pitch, right);
}

Camera::~Camera() {}
