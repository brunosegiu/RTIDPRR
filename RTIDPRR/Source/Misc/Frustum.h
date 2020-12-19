#pragma once

#include <array>
#include <glm/glm.hpp>

#include "AxisAlignedBoundingBox.h"

namespace RTIDPRR {
class Frustum {
 public:
  enum class FrustumPlanes { Left, Right, Top, Bottom, Back, Front };

  Frustum(const glm::mat4& matrix);

  void update(const glm::mat4& matrix);

  bool intersects(const glm::mat4 transform,
                  const AxisAlignedBoundingBox& boundingBox) const;

  virtual ~Frustum() = default;

 private:
  std::array<glm::vec4, 6> mPlanes;
};
}  // namespace RTIDPRR
