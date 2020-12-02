#include "Buffer.h"

#include "Context.h"
#include "DeviceMemory.h"

using namespace RTIDPRR::Graphics;

Buffer::Buffer(const vk::DeviceSize& size, const vk::BufferUsageFlags& usage,
               const vk::MemoryPropertyFlags& memoryProperties)
    : mSize(size) {
  const Device& device = Context::get().getDevice();
  vk::BufferCreateInfo bufferCreateInfo =
      vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(
          vk::SharingMode::eExclusive);
  mBuffer = device.getLogicalDeviceHandle().createBuffer(bufferCreateInfo);

  vk::MemoryRequirements memRequirements =
      device.getLogicalDeviceHandle().getBufferMemoryRequirements(mBuffer);
  vk::MemoryAllocateInfo memAllocInfo =
      vk::MemoryAllocateInfo()
          .setAllocationSize(memRequirements.size)
          .setMemoryTypeIndex(DeviceMemory::findMemoryIndex(
              memRequirements.memoryTypeBits, memoryProperties));
  mMemory = device.getLogicalDeviceHandle().allocateMemory(memAllocInfo);
  device.getLogicalDeviceHandle().bindBufferMemory(mBuffer, mMemory, 0);
}

Buffer::Buffer(Buffer&& other)
    : mBuffer(std::move(other.mBuffer)),
      mMemory(std::move(other.mMemory)),
      mSize(std::move(other.mSize)) {
  other.mBuffer = nullptr;
  other.mMemory = nullptr;
}

void Buffer::update(const void* value) {
  const Device& device = Context::get().getDevice();
  void* data = device.getLogicalDeviceHandle().mapMemory(mMemory, 0, mSize,
                                                         vk::MemoryMapFlags{});
  memcpy(data, value, mSize);
  device.getLogicalDeviceHandle().unmapMemory(mMemory);
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
      device.getLogicalDeviceHandle().allocateCommandBuffers(
          commandAllocInfo)[0];

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
  vk::Fence waitFence =
      device.getLogicalDeviceHandle().createFence(fenceCreateInfo);
  graphicsQueue.submit(submitInfo, waitFence);
  device.getLogicalDeviceHandle().waitForFences(
      waitFence, true, std::numeric_limits<uint64_t>::max());
  device.getLogicalDeviceHandle().freeCommandBuffers(
      graphicsQueue.getCommandPool(), commandBuffer);
  device.getLogicalDeviceHandle().destroyFence(waitFence);
}

Buffer::~Buffer() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyBuffer(mBuffer);
  device.getLogicalDeviceHandle().freeMemory(mMemory);
}
