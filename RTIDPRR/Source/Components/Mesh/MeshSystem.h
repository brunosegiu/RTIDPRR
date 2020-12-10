#pragma once

#include "../../Core/System.h"
#include "Mesh.h"

namespace RTIDPRR {
namespace Component {
class MeshSystem : public RTIDPRR::Core::System<Mesh> {
 public:
  MeshSystem();

  virtual void update(const float deltaTime) override;

  virtual ~MeshSystem();
};

}  // namespace Component
}  // namespace RTIDPRR
