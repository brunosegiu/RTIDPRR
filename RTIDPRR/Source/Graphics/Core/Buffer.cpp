#include "Buffer.h"

#include <cassert>

#include "Context.h"

using namespace RTIDPRR::Graphics;

Buffer::Buffer(const vk::DeviceSize& size, const vk::BufferUsageFlags& usage,
               const vk::MemoryPropertyFlags& memoryProperties)
    : mSize(size) {
  const Device& device = Context::get().getDevice();
  vk::BufferCreateInfo bufferCreateInfo =
      vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(
          vk::SharingMode::eExclusive);
  mBuffer = device.getLogicalDevice().createBuffer(bufferCreateInfo);

  vk::MemoryRequirements memRequirements =
      device.getLogicalDevice().getBufferMemoryRequirements(mBuffer);
  vk::MemoryAllocateInfo memAllocInfo =
      vk::MemoryAllocateInfo()
          .setAllocationSize(memRequirements.size)
          .setMemoryTypeIndex(findMemoryIndex(memRequirements.memoryTypeBits,
                                              memoryProperties));
  mMemory = device.getLogicalDevice().allocateMemory(memAllocInfo);
  device.getLogicalDevice().bindBufferMemory(mBuffer, mMemory, 0);
}

void Buffer::update(const void* value) {
  const Device& device = Context::get().getDevice();
  void* data = device.getLogicalDevice().mapMemory(mMemory, 0, mSize,
                                                   vk::MemoryMapFlags{});
  memcpy(data, value, mSize);
  device.getLogicalDevice().unmapMemory(mMemory);
}

void Buffer::copyInto(Buffer& other) {
  const Device& device = Context::get().getDevice();

  const Queue& graphicsQueue = device.getGraphicsQueue();

  vk::CommandBufferAllocateInfo commandAllocInfo =
      vk::CommandBufferAllocateInfo()
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandPool(graphicsQueue.getCommandPool())
          .setCommandBufferCount(1);
  vk::CommandBuffer commandBuffer =
      device.getLogicalDevice().allocateCommandBuffers(commandAllocInfo)[0];

  // Copy
  vk::CommandBufferBeginInfo commandBeginInfo =
      vk::CommandBufferBeginInfo().setFlags(
          vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  commandBuffer.begin(commandBeginInfo);

  vk::BufferCopy copyRegion =
      vk::BufferCopy().setSrcOffset(0).setDstOffset(0).setSize(mSize);
  commandBuffer.copyBuffer(mBuffer, other.mBuffer, copyRegion);
  commandBuffer.end();

  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(commandBuffer);

  vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo();
  vk::Fence waitFence = device.getLogicalDevice().createFence(fenceCreateInfo);
  graphicsQueue.submit(submitInfo, waitFence);
  device.getLogicalDevice().waitForFences(waitFence, true,
                                          std::numeric_limits<uint64_t>::max());
  device.getLogicalDevice().freeCommandBuffers(graphicsQueue.getCommandPool(),
                                               commandBuffer);
  device.getLogicalDevice().destroyFence(waitFence);
}

uint32_t Buffer::findMemoryIndex(const uint32_t filter,
                                 const vk::MemoryPropertyFlags properties) {
  const vk::PhysicalDevice& physicalDevice =
      Context::get().getDevice().getPhysicalDevice();
  vk::PhysicalDeviceMemoryProperties physicalMemoryProperties =
      physicalDevice.getMemoryProperties();

  for (uint32_t memoryIndex = 0;
       memoryIndex < physicalMemoryProperties.memoryTypeCount; ++memoryIndex) {
    if ((filter & (1 << memoryIndex)) &&
        (physicalMemoryProperties.memoryTypes[memoryIndex].propertyFlags &
         properties) == properties) {
      return memoryIndex;
    }
  }
  assert(false);
  return 0;
}
