#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Queue {
 public:
  Queue() = default;
  Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex,
        const vk::Device& device);

  const vk::CommandPool& getCommandPool() const { return mCommandPool; };

  void submit(const vk::SubmitInfo& submitInfo,
              const vk::Fence waitFence = nullptr) const;
  void present(const vk::PresentInfoKHR& presentInfo) const;

  virtual ~Queue();

 private:
  vk::Queue mHandle;
  uint32_t mFamilyIndex;
  uint32_t mIndex;
  vk::CommandPool mCommandPool;
};
}  // namespace Graphics
}  // namespace RTIDPRR
