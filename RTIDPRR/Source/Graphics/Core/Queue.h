#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
	namespace Graphics {
		class Queue {
		public:
			Queue() = default;
			Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex, const vk::Device& device);
			virtual ~Queue();

		private:
			vk::Queue mHandle;
			uint32_t mFamilyIndex;
			uint32_t mIndex;
		};
	} // namespace Graphics
} // namespace RTIDPRR
