#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace RTIDPRR::Component;

Transform::Transform(Object* object, Transform* parent)
    : Component(object),
      mAbsoluteTransform(1.0f),
      mLocalTransform(1.0f),
      mParent(parent),
      mLocalRotation(glm::vec3(0.0f)),
      mLocalScale(1.0f),
      mLocalTranslation(0.0f),
      mChildren() {
  if (mParent) {
    mParent->addChild(this);
  }
}

Transform::Transform(Transform&& other) noexcept
    : Component(std::move(other)),
      mAbsoluteTransform(std::move(other.mAbsoluteTransform)),
      mLocalTransform(std::move(other.mLocalTransform)),
      mParent(other.mParent),
      mLocalRotation(other.mLocalRotation),
      mLocalScale(other.mLocalScale),
      mLocalTranslation(other.mLocalTranslation),
      mChildren(std::move(other.mChildren)) {
  other.mParent = nullptr;
}

void Transform::rotate(const glm::vec3& value) {
  mLocalRotation *= glm::quat(value);
}

void Transform::rotate(const glm::quat& value) { mLocalRotation += value; }

void Transform::translate(const glm::vec3& value) {
  mLocalTranslation += value;
}

void Transform::scale(const glm::vec3& value) { mLocalScale += value; }

void Transform::setLocalRotation(const glm::quat& value) {
  mLocalRotation = value;
}

void Transform::setLocalTranslation(const glm::vec3& value) {
  mLocalTranslation = value;
}

void Transform::setLocalScale(const glm::vec3& value) { mLocalScale = value; }

void RTIDPRR::Component::Transform::addChild(Transform* transform) {
  mChildren.push_back(transform);
}

void Transform::update(const glm::mat4& parentAbsoluteMatrix) {
  mLocalTransform = glm::translate(glm::mat4(1.0f), mLocalTranslation);
  mLocalTransform *= glm::scale(glm::mat4(1.0f), mLocalScale);
  mLocalTransform *= glm::toMat4(glm::normalize(mLocalRotation));

  mAbsoluteTransform = mLocalTransform * parentAbsoluteMatrix;
  for (Transform* child : mChildren) {
    if (child != nullptr) {
      child->update(mAbsoluteTransform);
    }
  }
}

Transform::~Transform() {}
