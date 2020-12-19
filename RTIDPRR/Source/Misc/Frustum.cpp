#include "Frustum.h"

using namespace RTIDPRR;

Frustum::Frustum(const glm::mat4& matrix) : mPlanes() { update(matrix); }

void Frustum::update(const glm::mat4& matrix) {
  mPlanes[static_cast<uint32_t>(FrustumPlanes::Left)] =
      glm::vec4{matrix[0].w + matrix[0].x, matrix[1].w + matrix[1].x,
                matrix[2].w + matrix[2].x, matrix[3].w + matrix[3].x};

  mPlanes[static_cast<uint32_t>(FrustumPlanes::Right)] =
      glm::vec4{matrix[0].w - matrix[0].x, matrix[1].w - matrix[1].x,
                matrix[2].w - matrix[2].x, matrix[3].w - matrix[3].x};

  mPlanes[static_cast<uint32_t>(FrustumPlanes::Top)] =
      glm::vec4{matrix[0].w - matrix[0].y, matrix[1].w - matrix[1].y,
                matrix[2].w - matrix[2].y, matrix[3].w - matrix[3].y};

  mPlanes[static_cast<uint32_t>(FrustumPlanes::Bottom)] =
      glm::vec4{matrix[0].w + matrix[0].y, matrix[1].w + matrix[1].y,
                matrix[2].w + matrix[2].y, matrix[3].w + matrix[3].y};

  mPlanes[static_cast<uint32_t>(FrustumPlanes::Back)] =
      glm::vec4{matrix[0].w + matrix[0].z, matrix[1].w + matrix[1].z,
                matrix[2].w + matrix[2].z, matrix[3].w + matrix[3].z};

  mPlanes[static_cast<uint32_t>(FrustumPlanes::Front)] =
      glm::vec4{matrix[0].w - matrix[0].z, matrix[1].w - matrix[1].z,
                matrix[2].w - matrix[2].z, matrix[3].w - matrix[3].z};

  for (glm::vec4 plane : mPlanes) {
    glm::vec3 normal = glm::normalize(glm::vec3(plane));
    plane = glm::vec4(normal.x, normal.y, normal.z, plane.w);
  }
}

bool Frustum::intersects(const glm::mat4 transform,
                         const AxisAlignedBoundingBox& boundingBox) const {
  const glm::vec3& max = boundingBox.getMax();
  const glm::vec3& min = boundingBox.getMin();
  std::vector<glm::vec3> boxPoints{
      {min.x, min.y, min.z}, {max.x, min.y, min.z}, {max.x, max.y, min.z},
      {min.x, max.y, min.z}, {min.x, min.y, max.z}, {max.x, min.y, max.z},
      {max.x, max.y, max.z}, {min.x, max.y, max.z}};
  for (glm::vec3& point : boxPoints) {
    point = glm::vec3(transform * glm::vec4(point, 1.0f));
  }
  for (uint32_t planeIndex = 0; planeIndex < 6; ++planeIndex) {
    bool intersects = false;
    const glm::vec4 plane = mPlanes[planeIndex];
    for (const glm::vec3& point : boxPoints) {
      if (glm::dot(glm::vec4(point, 1.0f), plane) > 0) {
        intersects = true;
        break;
      }
    }
    if (!intersects) {
      return false;
    }
  }
  return true;
}
