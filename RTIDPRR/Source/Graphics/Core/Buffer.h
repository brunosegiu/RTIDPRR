#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Buffer {
 public:
  Buffer(const vk::DeviceSize& size, const vk::BufferUsageFlags& usage,
         const vk::MemoryPropertyFlags& memoryProperties);

  const vk::Buffer& getBufferHandle() const { return mBuffer; };

  void update(const void* value);

  virtual ~Buffer() = default;

 private:
  vk::Buffer mBuffer;
  vk::DeviceMemory mMemory;
  vk::DeviceSize mSize;

  static uint32_t findMemoryIndex(const uint32_t filter,
                                  const vk::MemoryPropertyFlags properties);
};
}  // namespace Graphics
}  // namespace RTIDPRR
