#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "../../Core/Component.h"

namespace RTIDPRR {
namespace Component {
class TransformSystem;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Component {
class Transform : public Component {
 public:
  using SystemType = TransformSystem;

  Transform(Object* object, Transform* parent = nullptr);
  Transform::Transform(Transform&& other) noexcept;

  glm::mat4& getAbsoluteTransform() { return mAbsoluteTransform; }
  glm::mat4& getLocalTransform() { return mLocalTransform; }
  Transform* getParent() { return mParent; }

  void rotate(const glm::vec3& value);
  void rotate(const glm::quat& value);
  void translate(const glm::vec3& value);
  void scale(const glm::vec3& value);

  void setLocalRotation(const glm::quat& value);
  void setLocalTranslation(const glm::vec3& value);
  void setLocalScale(const glm::vec3& value);

  void addChild(Transform* transform);

  void update(const glm::mat4& parentAbsoluteMatrix);

  virtual ~Transform();

 private:
  Transform* mParent;
  std::vector<Transform*> mChildren;

  glm::mat4 mAbsoluteTransform;
  glm::mat4 mLocalTransform;

  glm::vec3 mLocalTranslation;
  glm::quat mLocalRotation;
  glm::vec3 mLocalScale;
};

}  // namespace Component
}  // namespace RTIDPRR
