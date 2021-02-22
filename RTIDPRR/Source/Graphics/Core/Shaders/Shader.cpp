#include "Shader.h"

#include <filesystem>
#include <fstream>

#include "../../../Misc/DebugUtils.h"
#include "../Context.h"

using namespace RTIDPRR::Graphics;

Shader::Shader(const std::vector<char>& code,
               const vk::ShaderStageFlagBits stage)
    : mStage(stage) {
  vk::ShaderModuleCreateInfo shaderCreateInfo =
      vk::ShaderModuleCreateInfo()
          .setCodeSize(code.size() * sizeof(char))
          .setPCode(reinterpret_cast<const uint32_t*>(code.data()));
  const Device& device = Context::get().getDevice();
  mShaderHandle = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createShaderModule(shaderCreateInfo));
}

static const std::unordered_map<std::string, vk::ShaderStageFlagBits>
    shaderStageNames{
        {".vert", vk::ShaderStageFlagBits::eVertex},
        {".tesc", vk::ShaderStageFlagBits::eTessellationControl},
        {".tese", vk::ShaderStageFlagBits::eTessellationEvaluation},
        {".geom", vk::ShaderStageFlagBits::eGeometry},
        {".frag", vk::ShaderStageFlagBits::eFragment},
        {".comp", vk::ShaderStageFlagBits::eCompute},
    };

Shader* Shader::loadShader(const std::string& path) {
  const Device& device = Context::get().getDevice();
  // Check extension for stage
  // Expected name is <shader_name>.<shader_stage>.spirv
  const std::string extension =
      std::filesystem::path(path).extension().u8string();
  const auto it = shaderStageNames.find(extension);
  RTIDPRR_ASSERT_MSG(it != shaderStageNames.end(),
                     "Couldn't find matching shader extension");
  vk::ShaderStageFlagBits stage = it != shaderStageNames.end()
                                      ? it->second
                                      : vk::ShaderStageFlagBits::eVertex;
  // Load binary data
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> rawData(fileSize);
  if (file.read(rawData.data(), fileSize)) {
    // Create shader
    return new Shader(rawData, stage);
  }
  RTIDPRR_ASSERT_MSG(false, "Failed reading shader data");
  return nullptr;
}

Shader::~Shader() {
  Context::get().getDevice().getLogicalDeviceHandle().destroyShaderModule(
      mShaderHandle);
}
