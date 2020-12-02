#include "Queue.h"

#include "Context.h"

using namespace RTIDPRR::Graphics;

Queue::Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex,
             const vk::Device& logicalDevice)
    : mFamilyIndex(queueFamilyIndex),
      mIndex(queueIndex),
      mLogicalDeviceHandle(logicalDevice) {
  mHandle = logicalDevice.getQueue(queueFamilyIndex, queueIndex);

  vk::CommandPoolCreateInfo commandPoolCreateInfo =
      vk::CommandPoolCreateInfo()
          .setQueueFamilyIndex(queueFamilyIndex)
          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
  mCommandPool = logicalDevice.createCommandPool(commandPoolCreateInfo);
}

void Queue::submit(const vk::SubmitInfo& submitInfo,
                   const vk::Fence waitFence) const {
  mHandle.submit(submitInfo, waitFence);
}

void Queue::present(const vk::PresentInfoKHR& presentInfo) const {
  mHandle.presentKHR(presentInfo);
}

Queue::~Queue() { mLogicalDeviceHandle.destroyCommandPool(mCommandPool); }
