#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_set>

#include "../../Core/Component.h"

namespace RTIDPRR {
namespace Component {
class TransformSystem;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Component {
class Transform : public RTIDPRR::Core::Component {
 public:
  using SystemType = TransformSystem;

  Transform(RTIDPRR::Core::Object* object);
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

  const glm::vec3& getAbsoluteTranslation() { return mAbsoluteTranslation; };
  const glm::vec3& getAbsoluteScale() { return mAbsoluteScale; };
  const glm::quat& getAbsoluteRotation() { return mAbsoluteRotation; };

  void setParent(Transform* parent);

  void addChild(Transform* transform);
  void removeChild(Transform* child);

  void update(const glm::mat4& parentAbsoluteMatrix = glm::mat4(1.0f),
              const glm::vec3& parentAbsoluteTranslation = glm::vec3(0.0f),
              const glm::vec3& parentAbsoluteScale = glm::vec3(1.0f),
              const glm::quat& parentAbsoluteRotation = glm::quat(glm::vec3()));

  virtual ~Transform();

 private:
  Transform* mParent;
  std::unordered_set<Transform*> mChildren;

  glm::mat4 mLocalTransform;
  glm::vec3 mLocalTranslation;
  glm::quat mLocalRotation;
  glm::vec3 mLocalScale;

  glm::mat4 mAbsoluteTransform;
  glm::vec3 mAbsoluteTranslation;
  glm::quat mAbsoluteRotation;
  glm::vec3 mAbsoluteScale;
};

}  // namespace Component
}  // namespace RTIDPRR
