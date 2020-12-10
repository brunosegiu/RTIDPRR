#pragma once

#include <cstdint>

namespace RTIDPRR {
namespace Core {
class Scene;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Core {
class Object {
 public:
  Object(Scene* scene);

  Object(Object&& other) noexcept;

  template <class TComponent, typename... TArgs>
  TComponent& addComponent(TArgs&&... args);

  template <class TComponent>
  TComponent* getComponent();

  uint32_t getId() { return mId; }

  virtual ~Object();

 private:
  Scene* mScene;
  const uint32_t mId;
};

}  // namespace Core
}  // namespace RTIDPRR

using namespace RTIDPRR::Core;

template <class TComponent, typename... TArgs>
TComponent& Object::addComponent(TArgs&&... args) {
  Scene& scene = *mScene;
  TComponent::SystemType& system = scene.getSystem<TComponent::SystemType>();
  return system.addComponent(this, std::forward<TArgs>(args)...);
}

template <class TComponent>
TComponent* Object::getComponent() {
  Scene& scene = *mScene;
  TComponent::SystemType& system = scene.getSystem<TComponent::SystemType>();
  return system.getComponent(mId);
}
