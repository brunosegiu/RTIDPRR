#include "Device.h"

#include <utility>

using namespace RTIDPRR::Graphics;

Device::Device(const Instance& instance) {
	// Pick and initialize a physical device
	uint32_t deviceCount = 0;
	instance.getHandle().enumeratePhysicalDevices(&deviceCount, nullptr);
	std::vector<vk::PhysicalDevice> physicalDevices(deviceCount);
	instance.getHandle().enumeratePhysicalDevices(&deviceCount, physicalDevices.data());
	mPhysicalHandle = Device::findPhysicalDevice(physicalDevices);

	// Initialize the graphics and compute queues
	uint32_t queueFamilyCount = 0;
	mPhysicalHandle.getQueueFamilyProperties(&queueFamilyCount, nullptr);
	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
	mPhysicalHandle.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

	const uint32_t queueFamilyIndex = findQueueFamilyIndex(queueFamilies);

	std::vector<float> queuePriorities{
		1.0f, // Graphics queue
		0.5f // Compute queue
	};

	vk::DeviceQueueCreateInfo graphicsQueueCreateInfo =
		vk::DeviceQueueCreateInfo()
		.setQueueFamilyIndex(queueFamilyIndex)
		.setQueueCount(static_cast<uint32_t>(queuePriorities.size()))
		.setPQueuePriorities(queuePriorities.data());

	// Initialize the logical device
	vk::PhysicalDeviceFeatures deviceFeatures{};
	vk::DeviceCreateInfo deviceCreateInfo =
		vk::DeviceCreateInfo()
		.setPQueueCreateInfos(&graphicsQueueCreateInfo)
		.setQueueCreateInfoCount(1)
		.setPEnabledFeatures(&deviceFeatures);
	mDeviceHandle = mPhysicalHandle.createDevice(deviceCreateInfo);

	mGraphicsQueue = std::move(Queue(queueFamilyIndex, 0, mDeviceHandle));
	mComputeQueue = std::move(Queue(queueFamilyIndex, 1, mDeviceHandle));
}

const vk::PhysicalDevice Device::findPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices) {
	std::vector<uint32_t> scores(devices.size());
	uint32_t index = 0;
	for (const vk::PhysicalDevice& device : devices) {
		vk::PhysicalDeviceProperties properties = device.getProperties();
		vk::PhysicalDeviceFeatures features = device.getFeatures();
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			return device;
		}
	}
}

const uint32_t Device::findQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties) {
	for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index) {
		const vk::QueueFamilyProperties& queueFamilyProperty = queueFamilyProperties[index];
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics && queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute)
			return index;
	}
	assert(false);
	return 0;
}

Device::~Device() {}
