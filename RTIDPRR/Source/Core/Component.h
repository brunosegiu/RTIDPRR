#pragma once

namespace RTIDPRR {
namespace Core {
class Object;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Core {
class Component {
 public:
  Component(Object* object);
  Component(Component&& other) noexcept;

  Object* getObject() { return mPObject; }

  virtual ~Component();

 private:
  Object* mPObject;
};

}  // namespace Core
}  // namespace RTIDPRR
