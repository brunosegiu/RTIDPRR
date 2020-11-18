#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace RTIDPRR {
	namespace Graphics {
		class Shader {
		public:
			static const Shader* loadShader(const std::string& path);

			Shader(const Device& device, const std::vector<char>& code,
				const vk::ShaderStageFlagBits stage);

			const vk::ShaderModule& getModule() const { return mShaderHandle; }

			virtual ~Shader();

		private:
			vk::ShaderModule mShaderHandle;
			vk::ShaderStageFlagBits mStage;
		};
	} // namespace Graphics
} // namespace RTIDPRR
