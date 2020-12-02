#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Queue {
 public:
  Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex,
        const vk::Device& logicalDevice);

  const vk::CommandPool& getCommandPool() const { return mCommandPool; };
  const uint32_t getFamilyIndex() const { return mFamilyIndex; }

  void submit(const vk::SubmitInfo& submitInfo,
              const vk::Fence waitFence = nullptr) const;
  void present(const vk::PresentInfoKHR& presentInfo) const;

  virtual ~Queue();

 private:
  vk::Queue mHandle;
  uint32_t mFamilyIndex;
  uint32_t mIndex;
  vk::CommandPool mCommandPool;

  const vk::Device& mLogicalDeviceHandle;
};
}  // namespace Graphics
}  // namespace RTIDPRR
