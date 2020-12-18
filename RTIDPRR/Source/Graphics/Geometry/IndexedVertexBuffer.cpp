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
    : mVertexBuffer(std::move(initWithStagingBuffer(
          vertices, vk::BufferUsageFlagBits::eVertexBuffer))),
      mIndexBuffer(std::move(initWithStagingBuffer(
          indices, vk::BufferUsageFlagBits::eIndexBuffer))),
      mIndexCount(static_cast<uint32_t>(indices.size())),
      mVertexCount(static_cast<uint32_t>(vertices.size())) {}

IndexedVertexBuffer::IndexedVertexBuffer(IndexedVertexBuffer&& other)
    : mVertexBuffer(std::move(other.mVertexBuffer)),
      mIndexBuffer(std::move(other.mIndexBuffer)),
      mIndexCount(other.mIndexCount),
      mVertexCount(other.mVertexCount) {}
