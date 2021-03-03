#pragma once

#include <memory>
#include <unordered_set>
#include <vulkan/vulkan.hpp>

#include "Queue.h"

namespace RTIDPRR {
namespace Graphics {
class Command;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Graphics {
class CommandPool {
 public:
  CommandPool(const vk::Device& logicalDevice, Queue::QueueIndices indices);

  Command* allocateGraphicsCommand();
  Command* allocateComputeCommand();

  void releaseCommand(Command* command);

  virtual ~CommandPool();

 private:
  vk::CommandPool mGraphicsCommandPool;
  vk::CommandPool mComputeCommandPool;

  struct CommandCache {
    std::unordered_set<Command*> mUsedCommands;
    std::unordered_set<Command*> mUnusedCommands;
  };

  CommandCache mGraphicsCommandCache;
  CommandCache mComputeCommandCache;

  Command* allocateCommand(CommandCache& commandCache,
                           vk::CommandPool& commandPool);
};
}  // namespace Graphics
}  // namespace RTIDPRR
