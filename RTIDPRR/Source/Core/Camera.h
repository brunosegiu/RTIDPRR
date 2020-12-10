#pragma once

#include <glm/glm.hpp>

namespace RTIDPRR {
namespace Core {
class Camera {
 public:
  Camera();

  const glm::mat4& getViewProjection() const {
    updateViewProjection();
    return mVP;
  };

  const glm::vec3& getDirection() const { return mDirection; };
  const glm::vec3& getEye() const { return mEye; };

  void rotate(const float& angle, const glm::vec3& axis = glm::vec3(0, 1, 0));
  void rotate(const float& yaw, const float& pitch);

  void translate(const glm::vec3& value);
  void translateTo(const glm::vec3& value);

  void moveRight(const float& value);
  void moveLeft(const float& value);
  void moveForwards(const float& value);
  void moveBackwards(const float& value);

  virtual ~Camera();

 private:
  void updateViewProjection() const;

  glm::vec3 mEye;
  glm::vec3 mDirection;
  glm::vec3 mUp;

  glm::mat4 mProjection;
  mutable glm::mat4 mView;
  mutable glm::mat4 mVP;
};
}  // namespace Core
}  // namespace RTIDPRR
