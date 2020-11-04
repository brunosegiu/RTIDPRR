#include "Shader.h"

#include <fstream>
#include <filesystem>
#include <assert.h>

using namespace RTIDPRR::Graphics;

Shader::Shader(const Device& device, const std::vector<char>& code, const vk::ShaderStageFlagBits stage) : mStage(stage){
    vk::ShaderModuleCreateInfo shaderCreateInfo = 
        vk::ShaderModuleCreateInfo()
        .setCodeSize(code.size() * sizeof(char))
        .setPCode(reinterpret_cast<const unsigned int*>(code.data()));
    mShaderHandle = device.getLogicalDevice().createShaderModule(shaderCreateInfo);
}

static const std::unordered_map<std::string, vk::ShaderStageFlagBits> shaderStageNames {
    { ".vert", vk::ShaderStageFlagBits::eVertex },
    { ".tesc", vk::ShaderStageFlagBits::eTessellationControl },
    { ".tese", vk::ShaderStageFlagBits::eTessellationEvaluation },
    { ".geom", vk::ShaderStageFlagBits::eGeometry },
    { ".frag", vk::ShaderStageFlagBits::eFragment },
    { ".comp", vk::ShaderStageFlagBits::eCompute },
};

const Shader* Shader::loadShader(const Device& device, const std::string& path) {
    // Check extension for stage
    // Expected name is <shader_name>.<shader_stage>.spirv
    const std::string extension = std::filesystem::path(path).extension().u8string();
    const auto it = shaderStageNames.find(extension);
    assert(it != shaderStageNames.end());
    vk::ShaderStageFlagBits stage = it != shaderStageNames.end() ? it->second : vk::ShaderStageFlagBits::eVertex;
    // Load binary data
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> rawData(fileSize);
    if (file.read(rawData.data(), fileSize)) {
        // Create shader
        return new Shader(device, rawData, stage);
    }
    return nullptr;
}


Shader::~Shader() {

}