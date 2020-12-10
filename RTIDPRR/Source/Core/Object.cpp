﻿#include "Object.h"

#include "../Misc/Random/UUIDGenerator.h"

using namespace RTIDPRR::Core;

Object::Object(Scene* scene)
    : mScene(scene), mId(RTIDPRR::Random::UUIDGenerator::getId()) {}

Object::Object(Object&& other) noexcept : mId(other.mId), mScene(other.mScene) {
  other.mScene = nullptr;
}

Object::~Object() {}