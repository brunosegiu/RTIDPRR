#pragma once

namespace RTIDPRR {
namespace Core {
class Object;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Graphics {
class GizmoRenderer;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Core {
class Component {
 public:
  Component(Object* object);
  Component(Component&& other) noexcept;

  Object* getObject() const { return mPObject; }

  virtual void renderGizmos(
      RTIDPRR::Graphics::GizmoRenderer* renderer) const {};

  virtual ~Component();

 private:
  Object* mPObject;
};

}  // namespace Core
}  // namespace RTIDPRR
