#pragma once

#include <memory>
#include <unordered_set>
#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Command;
}
}  // namespace RTIDPRR

namespace RTIDPRR {
namespace Graphics {
class CommandPool {
 public:
  CommandPool(const vk::Device& logicalDevice, uint32_t graphicsFamilyIndex);

  Command* allocateCommand();
  void releaseCommand(Command* command);

  virtual ~CommandPool();

 private:
  vk::CommandPool mGraphicsCommandPool;

  std::unordered_set<Command*> mUsedCommands;
  std::unordered_set<Command*> mUnusedCommands;
};
}  // namespace Graphics
}  // namespace RTIDPRR
