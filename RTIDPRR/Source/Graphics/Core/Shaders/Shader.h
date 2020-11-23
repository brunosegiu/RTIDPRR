#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Shader {
 public:
  static const Shader* loadShader(const std::string& path);

  Shader(const std::vector<char>& code, const vk::ShaderStageFlagBits stage);

  const vk::ShaderModule& getModule() const { return mShaderHandle; }

  virtual ~Shader();

 private:
  vk::ShaderModule mShaderHandle;
  vk::ShaderStageFlagBits mStage;
};
}  // namespace Graphics
}  // namespace RTIDPRR
