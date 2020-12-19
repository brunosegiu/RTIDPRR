#include "AxisAlignedBoundingBox.h"

#include <limits>

using namespace RTIDPRR;

AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const std::vector<glm::vec3>& vertices)
    : mMaxBounds(std::numeric_limits<float>::min()),
      mMinBounds(std::numeric_limits<float>::max()) {
  for (const glm::vec3& vertex : vertices) {
    mMaxBounds = glm::max(mMaxBounds, vertex);
    mMinBounds = glm::min(mMinBounds, vertex);
  }
}
