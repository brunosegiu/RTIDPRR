﻿#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace RTIDPRR::Component;

const glm::vec3 Transform::sForward = glm::vec3(1.0f, 0.0f, 0.0f);

Transform::Transform(RTIDPRR::Core::Object* object)
    : Component(object),
      mAbsoluteTransform(1.0f),
      mLocalTransform(1.0f),
      mParent(nullptr),
      mLocalRotation(glm::vec3(0.0f)),
      mLocalScale(1.0f),
      mLocalTranslation(0.0f),
      mAbsoluteRotation(glm::vec3(0.0f)),
      mAbsoluteScale(1.0f),
      mAbsoluteTranslation(0.0f),
      mChildren() {}

Transform::Transform(Transform&& other) noexcept
    : Component(std::move(other)),
      mAbsoluteTransform(std::move(other.mAbsoluteTransform)),
      mLocalTransform(std::move(other.mLocalTransform)),
      mParent(other.mParent),
      mLocalRotation(other.mLocalRotation),
      mLocalScale(other.mLocalScale),
      mLocalTranslation(other.mLocalTranslation),
      mAbsoluteRotation(other.mAbsoluteRotation),
      mAbsoluteScale(other.mAbsoluteScale),
      mAbsoluteTranslation(other.mAbsoluteTranslation),
      mChildren(std::move(other.mChildren)) {
  other.mParent = nullptr;
}

void Transform::rotate(const glm::vec3& value) {
  mLocalRotation *= glm::quat(value);
}

void Transform::rotate(const glm::quat& value) { mLocalRotation *= value; }

void Transform::translate(const glm::vec3& value) {
  mLocalTranslation += value;
}

void Transform::scale(const glm::vec3& value) { mLocalScale *= value; }

void Transform::setLocalRotation(const glm::quat& value) {
  mLocalRotation = value;
}

void Transform::setLocalTranslation(const glm::vec3& value) {
  mLocalTranslation = value;
}

void Transform::setLocalScale(const glm::vec3& value) { mLocalScale = value; }

glm::vec3 Transform::getDirection() {
  const glm::quat& rotation = getAbsoluteRotation();
  const glm::vec4 lookDir =
      glm::mat4_cast(rotation) * glm::vec4(sForward, 1.0f);
  return glm::normalize(glm::vec3(lookDir.x, lookDir.y, lookDir.z));
}

void Transform::setParent(Transform* parent) {
  if (mParent) {
    mParent->removeChild(this);
  }
  mParent = parent;
  if (parent) {
    parent->addChild(this);
  }
}

void RTIDPRR::Component::Transform::addChild(Transform* transform) {
  mChildren.emplace(transform);
}

void Transform::removeChild(Transform* child) { mChildren.erase(child); }

void Transform::update(const glm::mat4& parentAbsoluteMatrix,
                       const glm::vec3& parentAbsoluteTranslation,
                       const glm::vec3& parentAbsoluteScale,
                       const glm::quat& parentAbsoluteRotation) {
  mLocalTransform = glm::translate(glm::mat4(1.0f), mLocalTranslation);
  mLocalTransform *= glm::mat4_cast(mLocalRotation);
  mLocalTransform *= glm::scale(glm::mat4(1.0f), mLocalScale);

  mAbsoluteTranslation = parentAbsoluteTranslation + mLocalTranslation;
  mAbsoluteScale = parentAbsoluteScale * mLocalScale;
  mAbsoluteRotation = parentAbsoluteRotation * mLocalRotation;

  mAbsoluteTransform = mLocalTransform * parentAbsoluteMatrix;
  for (Transform* child : mChildren) {
    if (child != nullptr) {
      child->update(mAbsoluteTransform, mAbsoluteTranslation, mAbsoluteScale,
                    mAbsoluteRotation);
    }
  }
}

Transform::~Transform() {}
