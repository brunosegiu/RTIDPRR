#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace RTIDPRR {
	namespace Graphics {
		class Pipeline {
		public:
			Pipeline(const Device& device);
			virtual ~Pipeline();

		private:
			vk::RenderPass mRenderPassHandle;
			vk::Pipeline mPipelineHandle;
			vk::PipelineLayout mLayoutHandle;
		};
	} // namespace Graphics
} // namespace RTIDPRR
