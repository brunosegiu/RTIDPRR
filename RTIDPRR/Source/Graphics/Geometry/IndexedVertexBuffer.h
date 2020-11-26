#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../Core/Buffer.h"

namespace RTIDPRR {
namespace Graphics {
class IndexedVertexBuffer {
 public:
  static const vk::IndexType getIndexType();
  static vk::VertexInputBindingDescription getBindingDescription();
  static vk::VertexInputAttributeDescription getAttributeDescription();

  IndexedVertexBuffer(const std::vector<glm::vec3>& vertices,
                      const std::vector<uint16_t>& indices);

  void draw(const vk::CommandBuffer& commandBuffer);

  virtual ~IndexedVertexBuffer() = default;

 private:
  Buffer mVertexBuffer;
  Buffer mIndexBuffer;
  const uint32_t mIndexCount, mVertexCount;
};
}  // namespace Graphics
}  // namespace RTIDPRR
