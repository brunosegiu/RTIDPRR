#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../Core/Buffer.h"

namespace RTIDPRR {
namespace Graphics {
class IndexedVertexBuffer {
 public:
  static const vk::IndexType getIndexType();

  IndexedVertexBuffer(const std::vector<glm::vec3>& vertices,
                      const std::vector<uint16_t>& indices);
  IndexedVertexBuffer(IndexedVertexBuffer&& other);
  IndexedVertexBuffer& operator=(IndexedVertexBuffer&& other) noexcept {
    return *this;
  }

  void draw(const vk::CommandBuffer& commandBuffer) const;

  virtual ~IndexedVertexBuffer() = default;

 private:
  IndexedVertexBuffer(const IndexedVertexBuffer&) = delete;
  IndexedVertexBuffer& operator=(const IndexedVertexBuffer&) = delete;

  Buffer mVertexBuffer;
  Buffer mIndexBuffer;
  const uint32_t mIndexCount, mVertexCount;
};
}  // namespace Graphics
}  // namespace RTIDPRR
