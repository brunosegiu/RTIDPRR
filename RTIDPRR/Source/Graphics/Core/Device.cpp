#include "Device.h"

#include <utility>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

Device::Device(const Instance& instance)
    : mGraphicsQueue(nullptr), mComputeQueue(nullptr) {
  // Pick and initialize a physical device
  const std::vector<vk::PhysicalDevice> physicalDevices =
      RTIDPRR_ASSERT_VK(instance.getHandle().enumeratePhysicalDevices());
  mPhysicalHandle = Device::findPhysicalDevice(physicalDevices);

  // Initialize the graphics and compute queues
  const std::vector<vk::QueueFamilyProperties> queueFamilies =
      mPhysicalHandle.getQueueFamilyProperties();

  mDeviceProperties = mPhysicalHandle.getProperties();

  const Queue::QueueIndices queueFamilyIndices =
      findQueueFamilyIndex(queueFamilies);

  std::vector<float> queuePriorities{1.0f, 0.5f};

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{
      vk::DeviceQueueCreateInfo()
          .setQueueFamilyIndex(queueFamilyIndices.graphicsQueueIndex)
          .setQueuePriorities(queuePriorities),
      vk::DeviceQueueCreateInfo()
          .setQueueFamilyIndex(queueFamilyIndices.computeQueueIndex)
          .setQueuePriorities(queuePriorities)};

  // Initialize the logical device
  std::vector<const char*> enabledExtensionNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  vk::DeviceCreateInfo deviceCreateInfo =
      vk::DeviceCreateInfo()
          .setQueueCreateInfos(queueCreateInfos)
          .setPEnabledExtensionNames(enabledExtensionNames);

  mLogicalHandle =
      RTIDPRR_ASSERT_VK(mPhysicalHandle.createDevice(deviceCreateInfo));

  mGraphicsQueue = std::make_unique<Queue>(
      queueFamilyIndices.graphicsQueueIndex, 0, mLogicalHandle);
  mComputeQueue = std::make_unique<Queue>(queueFamilyIndices.computeQueueIndex,
                                          0, mLogicalHandle);
}

const vk::PhysicalDevice Device::findPhysicalDevice(
    const std::vector<vk::PhysicalDevice>& devices) {
  std::vector<uint32_t> scores(devices.size());
  uint32_t index = 0;
  for (const vk::PhysicalDevice& device : devices) {
    vk::PhysicalDeviceProperties properties = device.getProperties();
    vk::PhysicalDeviceFeatures features = device.getFeatures();
    if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
      return device;
    }
  }
  RTIDPRR_ASSERT_MSG(false, "Couldn't find physical device");
  return nullptr;
}

const Queue::QueueIndices Device::findQueueFamilyIndex(
    const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties) {
  Queue::QueueIndices indices{Queue::QueueIndices::sInvalidIndex,
                              Queue::QueueIndices::sInvalidIndex};
  for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index) {
    const vk::QueueFamilyProperties& queueFamilyProperty =
        queueFamilyProperties[index];
    if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics &&
        queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute) {
      indices.graphicsQueueIndex = indices.computeQueueIndex = index;
      break;
    }
  }
  RTIDPRR_ASSERT_MSG(indices.graphicsQueueIndex != -1,
                     "Couldn't find graphics/compute queue");

  for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index) {
    const vk::QueueFamilyProperties& queueFamilyProperty =
        queueFamilyProperties[index];
    if (index != indices.graphicsQueueIndex &&
        queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute) {
      indices.computeQueueIndex = index;
    }
  }

  return indices;
}

Device::~Device() {
  mGraphicsQueue.reset();
  mComputeQueue.reset();
  mLogicalHandle.destroy();
}
