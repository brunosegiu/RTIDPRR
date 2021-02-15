#include "Mesh.h"

using namespace RTIDPRR::Component;

Mesh::Mesh(RTIDPRR::Core::Object* object,
           const std::vector<glm::vec3>& vertices,
           const std::vector<uint16_t>& indices)
    : Component(object) {
  mIndexedBuffer = std::make_unique<RTIDPRR::Graphics::IndexedVertexBuffer>(
      vertices, indices);
  mAABB = AxisAlignedBoundingBox(vertices);
}

Mesh::Mesh(Mesh&& other) noexcept
    : Component(std::move(other)),
      mIndexedBuffer(std::move(other.mIndexedBuffer)) {
  other.mIndexedBuffer = nullptr;
}

Mesh::~Mesh() {}
