#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Shader {
 public:
  static Shader* loadShader(const std::string& path);

  Shader(const std::vector<char>& code, const vk::ShaderStageFlagBits stage);
  Shader(Shader&&) = default;

  const vk::ShaderModule& getModule() const { return mShaderHandle; }
  const vk::ShaderStageFlagBits& getStage() const { return mStage; }

  virtual ~Shader();

 private:
  Shader(Shader const&) = delete;
  Shader& operator=(Shader const&) = delete;

  vk::ShaderModule mShaderHandle;
  vk::ShaderStageFlagBits mStage;
};
}  // namespace Graphics
}  // namespace RTIDPRR
