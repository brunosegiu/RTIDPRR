#include "IndexedVertexBuffer.h"

#include "../Core/Context.h"

using namespace RTIDPRR::Graphics;

const vk::IndexType IndexedVertexBuffer::getIndexType() {
  return vk::IndexType::eUint16;
}

template <typename T>
Buffer initWithStagingBuffer(const std::vector<T>& data,
                             const vk::BufferUsageFlags usage) {
  const Device& device = Context::get().getDevice();
  const size_t bufferSize = sizeof(T) * data.size();
  Buffer stagingBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                       vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent);
  stagingBuffer.update(data.data());
  Buffer hostLocalBuffer(bufferSize,
                         usage | vk::BufferUsageFlagBits::eTransferDst,
                         vk::MemoryPropertyFlagBits::eDeviceLocal);
  stagingBuffer.copyInto(hostLocalBuffer);
  return hostLocalBuffer;
}

IndexedVertexBuffer::IndexedVertexBuffer(const std::vector<glm::vec3>& vertices,
                                         const std::vector<uint16_t>& indices)
    : mVertexCount(static_cast<uint32_t>(vertices.size())),
      mIndexCount(static_cast<uint32_t>(indices.size())),
      mVertexBuffer(std::move(initWithStagingBuffer(
          vertices, vk::BufferUsageFlagBits::eVertexBuffer))),
      mIndexBuffer(std::move(initWithStagingBuffer(
          indices, vk::BufferUsageFlagBits::eIndexBuffer))) {}

void IndexedVertexBuffer::draw(const vk::CommandBuffer& commandBuffer) {
  vk::DeviceSize offset{0};
  commandBuffer.bindVertexBuffers(0, mVertexBuffer.getBufferHandle(), offset);
  commandBuffer.bindIndexBuffer(mIndexBuffer.getBufferHandle(), offset,
                                IndexedVertexBuffer::getIndexType());
  commandBuffer.drawIndexed(mIndexCount, 1, 0, 0, 0);
}
