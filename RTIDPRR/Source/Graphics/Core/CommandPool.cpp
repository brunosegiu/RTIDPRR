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
                         uint32_t graphicsFamilyIndex) {
  mGraphicsCommandPool =
      allocateCommandPool(logicalDevice, graphicsFamilyIndex);
}

Command* CommandPool::allocateCommand() {
  Command* command = nullptr;
  if (mUnusedCommands.size() > 0) {
    command = *mUnusedCommands.begin();
    mUnusedCommands.erase(command);
  } else {
    const Device& device = Context::get().getDevice();
    vk::CommandBufferAllocateInfo commandAllocInfo =
        vk::CommandBufferAllocateInfo()
            .setCommandPool(mGraphicsCommandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);

    std::vector<vk::CommandBuffer> commandBuffers = RTIDPRR_ASSERT_VK(
        device.getLogicalDeviceHandle().allocateCommandBuffers(
            commandAllocInfo));
    command = new Command(commandBuffers[0]);
  }
  mUsedCommands.emplace(command);
  return command;
}

void CommandPool::releaseCommand(Command* command) {
  mUsedCommands.erase(command);
  mUnusedCommands.emplace(command);
}

CommandPool::~CommandPool() {
  const Device& device = Context::get().getDevice();

  for (Command* command : mUsedCommands) {
    device.getLogicalDeviceHandle().freeCommandBuffers(mGraphicsCommandPool,
                                                       *command);
    delete command;
  }

  for (Command* command : mUnusedCommands) {
    device.getLogicalDeviceHandle().freeCommandBuffers(mGraphicsCommandPool,
                                                       *command);
    delete command;
  }

  device.getLogicalDeviceHandle().destroyCommandPool(mGraphicsCommandPool);
}
