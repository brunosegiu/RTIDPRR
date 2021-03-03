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

  void update(const void* value, const vk::DeviceSize& offset = 0,
              const vk::DeviceSize& size = Buffer::sInvalidSize);
  void* map();
  void unmap();
  void copyInto(Buffer& other);

  virtual ~Buffer();

  static const vk::DeviceSize sInvalidSize;

 private:
  vk::Buffer mBuffer;
  vk::DeviceMemory mMemory;
  vk::DeviceSize mSize;
  void* mPMapped;
};
}  // namespace Graphics
}  // namespace RTIDPRR
