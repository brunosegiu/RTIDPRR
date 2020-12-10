#pragma once

#include "../../Core/System.h"
#include "Transform.h"

namespace RTIDPRR {
namespace Component {
class TransformSystem : public RTIDPRR::Core::System<Transform> {
 public:
  TransformSystem();

  virtual void update(const float deltaTime) override;

  virtual ~TransformSystem();
};

}  // namespace Component
}  // namespace RTIDPRR
