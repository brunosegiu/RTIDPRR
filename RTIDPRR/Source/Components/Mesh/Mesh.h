#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "../../Core/Component.h"
#include "../../Core/System.h"
#include "../../Graphics/Geometry/IndexedVertexBuffer.h"

namespace RTIDPRR {
namespace Component {
class Mesh : public RTIDPRR::Core::Component {
 public:
  using SystemType = RTIDPRR::Core::System<Mesh>;

  Mesh(RTIDPRR::Core::Object* object, const std::string& filePath);
  Mesh(Mesh&& other) noexcept;

  void draw(vk::CommandBuffer& commandBuffer);

  ~Mesh();

 private:
  std::unique_ptr<RTIDPRR::Graphics::IndexedVertexBuffer> mIndexedBuffer;
};

}  // namespace Component
}  // namespace RTIDPRR
