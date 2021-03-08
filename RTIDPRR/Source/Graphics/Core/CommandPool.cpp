#include "CommandPool.h"

#include "../../Misc/DebugUtils.h"
#include "Command.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

vk::CommandPool allocateCommandPool(const vk::Device& logicalDevice,
                                    uint32_t queueFamilyIndex) {
  vk::CommandPoolCreateInfo commandPoolCreateInfo =
      vk::CommandPoolCreateInfo()
          .setQueueFamilyIndex(queueFamilyIndex)
          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
  return RTIDPRR_ASSERT_VK(
      logicalDevice.createCommandPool(commandPoolCreateInfo));
}

CommandPool::CommandPool(const vk::Device& logicalDevice,
                         Queue::QueueIndices indices) {
  mGraphicsCommandPool =
      allocateCommandPool(logicalDevice, indices.graphicsQueueIndex);
  mComputeCommandPool =
      allocateCommandPool(logicalDevice, indices.computeQueueIndex);
}

Command* CommandPool::allocateGraphicsCommand() {
  return allocateCommand(mGraphicsCommandCache, mGraphicsCommandPool);
}

Command* CommandPool::allocateComputeCommand() {
  return allocateCommand(mComputeCommandCache, mComputeCommandPool);
}

Command* CommandPool::allocateCommand(CommandCache& commandCache,
                                      vk::CommandPool& commandPool) {
  Command* command = nullptr;
  if (commandCache.mUnusedCommands.size() > 0) {
    command = *commandCache.mUnusedCommands.begin();
    commandCache.mUnusedCommands.erase(command);
  } else {
    const Device& device = Context::get().getDevice();
    vk::CommandBufferAllocateInfo commandAllocInfo =
        vk::CommandBufferAllocateInfo()
            .setCommandPool(commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);

    std::vector<vk::CommandBuffer> commandBuffers = RTIDPRR_ASSERT_VK(
        device.getLogicalDeviceHandle().allocateCommandBuffers(
            commandAllocInfo));
    command = new Command(commandBuffers[0]);
  }
  commandCache.mUsedCommands.emplace(command);
  return command;
}

void CommandPool::releaseCommand(Command* command) {
  if (mGraphicsCommandCache.mUsedCommands.find(command) !=
      mGraphicsCommandCache.mUsedCommands.end()) {
    mGraphicsCommandCache.mUsedCommands.erase(command);
    mGraphicsCommandCache.mUnusedCommands.emplace(command);
  } else {
    mComputeCommandCache.mUsedCommands.erase(command);
    mComputeCommandCache.mUnusedCommands.emplace(command);
  }
}

CommandPool::~CommandPool() {
  const Device& device = Context::get().getDevice();

  for (Command* command : mGraphicsCommandCache.mUsedCommands) {
    device.getLogicalDeviceHandle().freeCommandBuffers(mGraphicsCommandPool,
                                                       *command);
    delete command;
  }

  for (Command* command : mGraphicsCommandCache.mUnusedCommands) {
    device.getLogicalDeviceHandle().freeCommandBuffers(mGraphicsCommandPool,
                                                       *command);
    delete command;
  }

  device.getLogicalDeviceHandle().destroyCommandPool(mGraphicsCommandPool);

  for (Command* command : mComputeCommandCache.mUsedCommands) {
    device.getLogicalDeviceHandle().freeCommandBuffers(mComputeCommandPool,
                                                       *command);
    delete command;
  }

  for (Command* command : mComputeCommandCache.mUnusedCommands) {
    device.getLogicalDeviceHandle().freeCommandBuffers(mComputeCommandPool,
                                                       *command);
    delete command;
  }

  device.getLogicalDeviceHandle().destroyCommandPool(mComputeCommandPool);
}
