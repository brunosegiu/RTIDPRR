#pragma once

#include <vulkan/vulkan.hpp>

#include "Instance.h"
#include "Queue.h"

namespace RTIDPRR {
namespace Graphics {
class Device {
 public:
  Device(const Instance& instance);

  const Queue& getGraphicsQueue() const { return *mGraphicsQueue; };

  const vk::PhysicalDevice& getPhysicalDeviceHandle() const {
    return mPhysicalHandle;
  }
  const vk::Device& getLogicalDeviceHandle() const { return mLogicalHandle; }

  virtual ~Device();

 private:
  Device(Device const&) = delete;
  Device& operator=(Device const&) = delete;

  static const vk::PhysicalDevice findPhysicalDevice(
      const std::vector<vk::PhysicalDevice>& devices);
  static const uint32_t findQueueFamilyIndex(
      const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties);

  vk::PhysicalDevice mPhysicalHandle;
  vk::Device mLogicalHandle;

  std::unique_ptr<Queue> mGraphicsQueue;
  std::unique_ptr<Queue> mComputeQueue;
};
}  // namespace Graphics
}  // namespace RTIDPRR
