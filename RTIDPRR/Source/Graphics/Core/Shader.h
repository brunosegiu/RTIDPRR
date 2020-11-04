#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace RTIDPRR {
	namespace Graphics {
		class Shader {
		public:
			Shader(const Device& device, const std::vector<char>& code,
				const vk::ShaderStageFlagBits stage);

			static const Shader* loadShader(const Device& device, const std::string& path);

			virtual ~Shader();

		private:
			vk::ShaderModule mShaderHandle;
			vk::ShaderStageFlagBits mStage;
		};
	} // namespace Graphics
} // namespace RTIDPRR
