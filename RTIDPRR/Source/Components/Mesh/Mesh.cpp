#include "Mesh.h"

using namespace RTIDPRR::Component;

uint32_t Mesh::sLastIndex = 0;

Mesh::Mesh(RTIDPRR::Core::Object* object,
           const std::vector<glm::vec3>& vertices,
           const std::vector<uint16_t>& indices)
    : Component(object), mStartingIndex(0) {
  mIndexedBuffer = std::make_unique<RTIDPRR::Graphics::IndexedVertexBuffer>(
      vertices, indices);
  mAABB = AxisAlignedBoundingBox(vertices);
  mStartingIndex = sLastIndex;
  sLastIndex += static_cast<uint32_t>(indices.size()) / 3;
}

Mesh::Mesh(Mesh&& other) noexcept
    : Component(std::move(other)),
      mIndexedBuffer(std::move(other.mIndexedBuffer)) {
  other.mIndexedBuffer = nullptr;
}

Mesh::~Mesh() {}
