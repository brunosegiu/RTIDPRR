#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Buffer {
 public:
  Buffer(const vk::DeviceSize& size, const vk::BufferUsageFlags& usage,
         const vk::MemoryPropertyFlags& memoryProperties);

  Buffer(Buffer&& other);

  const vk::Buffer& getBufferHandle() const { return mBuffer; };

  void update(const void* value);
  void copyInto(Buffer& other);

  virtual ~Buffer();

 private:
  vk::Buffer mBuffer;
  vk::DeviceMemory mMemory;
  vk::DeviceSize mSize;
};
}  // namespace Graphics
}  // namespace RTIDPRR
