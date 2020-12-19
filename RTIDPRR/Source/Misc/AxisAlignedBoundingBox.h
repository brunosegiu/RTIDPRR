#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace RTIDPRR {
class AxisAlignedBoundingBox {
 public:
  AxisAlignedBoundingBox(const std::vector<glm::vec3>& vertices = {});

  const glm::vec3& getMax() const { return mMaxBounds; }
  const glm::vec3& getMin() const { return mMinBounds; }

  virtual ~AxisAlignedBoundingBox() = default;

 private:
  glm::vec3 mMaxBounds;
  glm::vec3 mMinBounds;
};
}  // namespace RTIDPRR
