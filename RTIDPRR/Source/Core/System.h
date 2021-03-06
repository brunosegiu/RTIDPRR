﻿#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace RTIDPRR {
namespace Core {
class Object;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Core {
template <class TComponent>
class System {
 public:
  System();

  virtual void update(const float deltaTime){};

  template <typename... TArgs>
  TComponent* addComponent(Object* object, TArgs&&... args);

  TComponent* getComponent(const uint32_t id);
  const TComponent* getComponent(const uint32_t id) const;

  std::vector<TComponent>& getComponents() { return mComponents; }

  virtual ~System();

  constexpr static size_t sInitialSize = 50;

 protected:
  std::vector<TComponent> mComponents;
  std::unordered_map<uint32_t, size_t> mObjectIdToComponent;
};
}  // namespace Core
}  // namespace RTIDPRR

template <class TComponent>
RTIDPRR::Core::System<TComponent>::System()
    : mComponents(), mObjectIdToComponent() {
  mComponents.reserve(sInitialSize);
  mObjectIdToComponent.reserve(sInitialSize);
}

template <class TComponent>
template <typename... TArgs>
TComponent* RTIDPRR::Core::System<TComponent>::addComponent(Object* object,
                                                            TArgs&&... args) {
  TComponent& newComponent =
      mComponents.emplace_back(object, std::forward<TArgs>(args)...);
  mObjectIdToComponent.emplace(object->getId(), mComponents.size() - 1);
  return &newComponent;
}

template <class TComponent>
TComponent* RTIDPRR::Core::System<TComponent>::getComponent(const uint32_t id) {
  auto it = mObjectIdToComponent.find(id);
  if (it != mObjectIdToComponent.end()) {
    return &mComponents[it->second];
  } else {
    return nullptr;
  }
}

template <class TComponent>
const TComponent* RTIDPRR::Core::System<TComponent>::getComponent(
    const uint32_t id) const {
  auto it = mObjectIdToComponent.find(id);
  if (it != mObjectIdToComponent.end()) {
    return &mComponents[it->second];
  } else {
    return nullptr;
  }
}

template <class TComponent>
RTIDPRR::Core::System<TComponent>::~System() {}
