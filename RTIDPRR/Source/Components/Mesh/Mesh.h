#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "../../Core/Component.h"
#include "../../Core/System.h"
#include "../../Graphics/Geometry/IndexedVertexBuffer.h"
#include "../../Misc/AxisAlignedBoundingBox.h"

namespace RTIDPRR {
namespace Component {
class Mesh : public RTIDPRR::Core::Component {
 public:
  using SystemType = RTIDPRR::Core::System<Mesh>;

  Mesh(RTIDPRR::Core::Object* object, const std::vector<glm::vec3>& vertices,
       const std::vector<uint16_t>& indices);
  Mesh(Mesh&& other) noexcept;

  const RTIDPRR::Graphics::IndexedVertexBuffer& getIndexedBuffer() {
    return *mIndexedBuffer;
  }
  const AxisAlignedBoundingBox& getAABB() const { return mAABB; }
  const uint32_t getStartingIndex() const { return mStartingIndex; }

  ~Mesh();

 private:
  std::unique_ptr<RTIDPRR::Graphics::IndexedVertexBuffer> mIndexedBuffer;
  AxisAlignedBoundingBox mAABB;

  static uint32_t sLastIndex;
  uint32_t mStartingIndex;
};

}  // namespace Component
}  // namespace RTIDPRR
