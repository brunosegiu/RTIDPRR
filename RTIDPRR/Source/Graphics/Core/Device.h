#pragma once

#include <vulkan/vulkan.hpp>

#include "Instance.h"
#include "Queue.h"

namespace RTIDPRR {
namespace Graphics {
class Device {
 public:
  Device(const Instance& instance);

  const Queue& getGraphicsQueue() const { return mGraphicsQueue; };

  const vk::PhysicalDevice& getPhysicalDevice() const {
    return mPhysicalHandle;
  }
  const vk::Device& getLogicalDevice() const { return mLogicalHandle; }

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

  Queue mGraphicsQueue;
  Queue mComputeQueue;
};
}  // namespace Graphics
}  // namespace RTIDPRR
