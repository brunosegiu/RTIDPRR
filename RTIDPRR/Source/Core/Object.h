#pragma once

#include <cstdint>
#include <list>

namespace RTIDPRR {
namespace Core {
class Scene;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Graphics {
class GizmoRenderer;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Core {
class Component;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Core {
class Object {
 public:
  Object(Scene* scene);

  Object(Object&& other) noexcept;

  template <class TComponent, typename... TArgs>
  TComponent* addComponent(TArgs&&... args);

  template <class TComponent>
  TComponent* getComponent();

  template <class TComponent>
  const TComponent* getComponent() const;

  Scene* getScene() { return mScene; }

  uint32_t getId() { return mId; }

  void renderGizmos(RTIDPRR::Graphics::GizmoRenderer* renderer) const;

  virtual ~Object();

 private:
  Scene* mScene;
  const uint32_t mId;

  std::list<RTIDPRR::Core::Component*> mComponents;
};

}  // namespace Core
}  // namespace RTIDPRR

#include "Scene.h"

using namespace RTIDPRR::Core;

template <class TComponent, typename... TArgs>
TComponent* Object::addComponent(TArgs&&... args) {
  Scene& scene = *mScene;
  typename TComponent::SystemType& system =
      scene.getSystem<typename TComponent::SystemType>();
  TComponent* component =
      system.addComponent(this, std::forward<TArgs>(args)...);
  mComponents.push_back(component);
  return component;
}

template <class TComponent>
TComponent* Object::getComponent() {
  Scene& scene = *mScene;
  typename TComponent::SystemType& system =
      scene.getSystem<typename TComponent::SystemType>();
  return system.getComponent(mId);
}

template <class TComponent>
const TComponent* Object::getComponent() const {
  Scene& scene = *mScene;
  typename TComponent::SystemType& system =
      scene.getSystem<typename TComponent::SystemType>();
  return system.getComponent(mId);
}
