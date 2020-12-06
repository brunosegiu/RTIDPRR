#include "Device.h"

#include <utility>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

Device::Device(const Instance& instance)
    : mGraphicsQueue(nullptr), mComputeQueue(nullptr) {
  // Pick and initialize a physical device
  const std::vector<vk::PhysicalDevice> physicalDevices =
      instance.getHandle().enumeratePhysicalDevices();
  mPhysicalHandle = Device::findPhysicalDevice(physicalDevices);

  // Initialize the graphics and compute queues
  const std::vector<vk::QueueFamilyProperties> queueFamilies =
      mPhysicalHandle.getQueueFamilyProperties();

  const uint32_t queueFamilyIndex = findQueueFamilyIndex(queueFamilies);

  std::vector<float> queuePriorities{
      1.0f,  // Graphics queue
      0.5f   // Compute queue
  };

  vk::DeviceQueueCreateInfo graphicsQueueCreateInfo =
      vk::DeviceQueueCreateInfo()
          .setQueueFamilyIndex(queueFamilyIndex)
          .setQueueCount(static_cast<uint32_t>(queuePriorities.size()))
          .setPQueuePriorities(queuePriorities.data());

  // Initialize the logical device
  std::vector<const char*> enabledExtensionNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  vk::DeviceCreateInfo deviceCreateInfo =
      vk::DeviceCreateInfo()
          .setQueueCreateInfos(graphicsQueueCreateInfo)
          .setPEnabledExtensionNames(enabledExtensionNames);

  mLogicalHandle =
      RTIDPRR_ASSERT_VK(mPhysicalHandle.createDevice(deviceCreateInfo));

  mGraphicsQueue = std::make_unique<Queue>(queueFamilyIndex, 0, mLogicalHandle);
  mComputeQueue = std::make_unique<Queue>(queueFamilyIndex, 1, mLogicalHandle);
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

const uint32_t Device::findQueueFamilyIndex(
    const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties) {
  for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index) {
    const vk::QueueFamilyProperties& queueFamilyProperty =
        queueFamilyProperties[index];
    if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics &&
        queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute)
      return index;
  }
  RTIDPRR_ASSERT_MSG(false, "Couldn't find graphics/compute queue");
  return 0;
}

Device::~Device() {
  mGraphicsQueue.reset();
  mComputeQueue.reset();
  mLogicalHandle.destroy();
}
