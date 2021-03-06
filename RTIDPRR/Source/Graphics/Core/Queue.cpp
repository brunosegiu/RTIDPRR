﻿#include "Queue.h"

#include "../../Misc/DebugUtils.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

Queue::Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex,
             const vk::Device& logicalDevice)
    : mFamilyIndex(queueFamilyIndex),
      mIndex(queueIndex),
      mLogicalDeviceHandle(logicalDevice) {
  mHandle = logicalDevice.getQueue(queueFamilyIndex, queueIndex);
}

void Queue::submit(const vk::SubmitInfo& submitInfo,
                   const vk::Fence waitFence) const {
  RTIDPRR_ASSERT_VK(mHandle.submit(submitInfo, waitFence));
}

vk::Result Queue::present(const vk::PresentInfoKHR& presentInfo) const {
  return mHandle.presentKHR(presentInfo);
}

Queue::~Queue() {}
