#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "../../Core/Component.h"
#include "../../Graphics/Geometry/IndexedVertexBuffer.h"

namespace RTIDPRR {
namespace Component {
class MeshSystem;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Component {
class Mesh : public Component {
 public:
  using SystemType = MeshSystem;

  Mesh(Object* object, const std::string& filePath);
  Mesh::Mesh(Mesh&& other) noexcept;

  void draw(vk::CommandBuffer& commandBuffer);

  ~Mesh();

 private:
  std::unique_ptr<RTIDPRR::Graphics::IndexedVertexBuffer> mIndexedBuffer;
};

}  // namespace Component
}  // namespace RTIDPRR
