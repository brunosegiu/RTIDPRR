#include "ComputePipeline.h"

#include "../../Misc/DebugUtils.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

ComputePipeline::ComputePipeline(
    const std::string& shaderPath,
    const std::vector<vk::DescriptorSetLayout>& descriptorLayouts,
    const std::vector<vk::PushConstantRange>& pushConstants,
    const SpecializationMap& specializationConstants) {
  const Device& device = Context::get().getDevice();

  // Shader parameter layout
  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo =
      vk::PipelineLayoutCreateInfo()
          .setSetLayouts(descriptorLayouts)
          .setPushConstantRanges(pushConstants);

  mLayoutHandle =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createPipelineLayout(
          pipelineLayoutCreateInfo));

  // Load and bind shaders
  vk::SpecializationInfo specializationInfo;

  static const char* entryPointName = "main";
  mShader = Shader::loadShader(shaderPath);
  const auto& constantInfo =
      specializationConstants.find(vk::ShaderStageFlagBits::eCompute);
  bool hasSpecializationConstants =
      constantInfo != specializationConstants.end();

  if (hasSpecializationConstants) {
    const SpecializationInfo& info = constantInfo->second;
    specializationInfo.setPData(info.data).setDataSize(info.size).setMapEntries(
        info.entries);
  }
  vk::PipelineShaderStageCreateInfo stageInfo =
      vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eCompute)
          .setModule(mShader->getModule())
          .setPName(entryPointName)
          .setPSpecializationInfo(
              hasSpecializationConstants ? &specializationInfo : nullptr);

  vk::ComputePipelineCreateInfo pipelineCreateInfo =
      vk::ComputePipelineCreateInfo().setStage(stageInfo).setLayout(
          mLayoutHandle);

  mPipelineHandle =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createComputePipeline(
          nullptr, pipelineCreateInfo));
  delete mShader;
}

ComputePipeline::~ComputePipeline() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyPipelineLayout(mLayoutHandle);
  device.getLogicalDeviceHandle().destroyPipeline(mPipelineHandle);
}
