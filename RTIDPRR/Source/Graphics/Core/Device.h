#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
	namespace Graphics {
		class Device {
		public:
			Device();

			virtual ~Device();

		private:
			Device(Device const&) = delete;
			Device& operator=(Device const&) = delete;

			vk::PhysicalDevice mPhysicalHandle;
			vk::Device mDeviceHandle;
		};
	} // namespace Graphics
} // namespace RTIDPRR
