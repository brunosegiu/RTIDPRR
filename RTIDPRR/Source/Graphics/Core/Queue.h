#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Queue {
 public:
  struct QueueIndices {
    uint32_t graphicsQueueIndex;
    uint32_t computeQueueIndex;

    static constexpr uint32_t sInvalidIndex = static_cast<uint32_t>(-1);
  };

  Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex,
        const vk::Device& logicalDevice);

  const uint32_t getFamilyIndex() const { return mFamilyIndex; }

  void submit(const vk::SubmitInfo& submitInfo,
              const vk::Fence waitFence = nullptr) const;
  vk::Result present(const vk::PresentInfoKHR& presentInfo) const;

  virtual ~Queue();

 private:
  vk::Queue mHandle;
  uint32_t mFamilyIndex;
  uint32_t mIndex;

  const vk::Device& mLogicalDeviceHandle;
};
}  // namespace Graphics
}  // namespace RTIDPRR
