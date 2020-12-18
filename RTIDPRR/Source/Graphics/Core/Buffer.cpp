#include "Buffer.h"

#include "Command.h"
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
  mBuffer = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createBuffer(bufferCreateInfo));

  vk::MemoryRequirements memRequirements =
      device.getLogicalDeviceHandle().getBufferMemoryRequirements(mBuffer);
  vk::MemoryAllocateInfo memAllocInfo =
      vk::MemoryAllocateInfo()
          .setAllocationSize(memRequirements.size)
          .setMemoryTypeIndex(DeviceMemory::findMemoryIndex(
              memRequirements.memoryTypeBits, memoryProperties));
  mMemory = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().allocateMemory(memAllocInfo));
  RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().bindBufferMemory(mBuffer, mMemory, 0));
}

Buffer::Buffer(Buffer&& other)
    : mBuffer(std::move(other.mBuffer)),
      mMemory(std::move(other.mMemory)),
      mSize(std::move(other.mSize)) {
  other.mBuffer = nullptr;
  other.mMemory = nullptr;
}

const vk::DeviceSize Buffer::sInvalidSize = static_cast<vk::DeviceSize>(-1);

void Buffer::update(const void* value, const vk::DeviceSize& offset,
                    const vk::DeviceSize& size) {
  const Device& device = Context::get().getDevice();
  vk::DeviceSize effectiveSize = size == Buffer::sInvalidSize ? mSize : size;
  void* data = RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().mapMemory(
      mMemory, offset, effectiveSize, vk::MemoryMapFlags{}));
  memcpy(data, value, effectiveSize);
  device.getLogicalDeviceHandle().unmapMemory(mMemory);
}

void Buffer::copyInto(Buffer& other) {
  const Device& device = Context::get().getDevice();

  const Queue& graphicsQueue = device.getGraphicsQueue();

  Command* command = Context::get().getCommandPool().allocateCommand();

  // Copy
  vk::CommandBufferBeginInfo commandBeginInfo =
      vk::CommandBufferBeginInfo().setFlags(
          vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  RTIDPRR_ASSERT_VK(command->begin(commandBeginInfo));

  vk::BufferCopy copyRegion =
      vk::BufferCopy().setSrcOffset(0).setDstOffset(0).setSize(mSize);
  command->copyBuffer(mBuffer, other.mBuffer, copyRegion);
  RTIDPRR_ASSERT_VK(command->end());

  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(
      *static_cast<vk::CommandBuffer*>(command));

  vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo();
  vk::Fence waitFence = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createFence(fenceCreateInfo));
  graphicsQueue.submit(submitInfo, waitFence);
  vk::Result copyRes = device.getLogicalDeviceHandle().waitForFences(
      waitFence, true, std::numeric_limits<uint64_t>::max());
  RTIDPRR_ASSERT(copyRes == vk::Result::eSuccess);
  device.getLogicalDeviceHandle().destroyFence(waitFence);
}

Buffer::~Buffer() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyBuffer(mBuffer);
  device.getLogicalDeviceHandle().freeMemory(mMemory);
}
