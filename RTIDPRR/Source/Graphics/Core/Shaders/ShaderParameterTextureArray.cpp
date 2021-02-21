#include "ShaderParameterTextureArray.h"

#include "../../../Misc/DebugUtils.h"
#include "../Context.h"

using namespace RTIDPRR::Graphics;

ShaderParameterTextureArray::ShaderParameterTextureArray(
    const std::vector<SamplerOptions>& samplerOptions)
    : mSamplers(samplerOptions.size()), mTextures(samplerOptions.size()) {
  const Device& device = Context::get().getDevice();
  for (uint32_t index = 0; index < samplerOptions.size(); ++index) {
    const SamplerOptions& options = samplerOptions[index];
    mTextures[index] = options.texture;
    vk::SamplerCreateInfo samplerCreateInfo =
        vk::SamplerCreateInfo()
            .setMagFilter(options.filter)
            .setMinFilter(options.filter)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setAddressModeU(options.clampMode)
            .setAddressModeV(options.clampMode)
            .setAddressModeW(options.clampMode)
            .setMipLodBias(0.0f)
            .setMaxAnisotropy(1.0f)
            .setMinLod(0.0f)
            .setMaxLod(1.0f)
            .setBorderColor(options.borderColor);

    mSamplers[index] = RTIDPRR_ASSERT_VK(
        device.getLogicalDeviceHandle().createSampler(samplerCreateInfo));
  }
}

ShaderParameterTextureArray::ShaderParameterTextureArray(
    ShaderParameterTextureArray&& other)
    : mSamplers(std::move(other.mSamplers)),
      mTextures(std::move(other.mTextures)) {
  other.mSamplers = {};
}

void ShaderParameterTextureArray::bindToGroup(
    const vk::DescriptorSet& descriptorSet, const uint32_t binding) {
  std::vector<vk::DescriptorImageInfo> descriptorImageInfos(mTextures.size());
  for (uint32_t index = 0; index < mTextures.size(); ++index) {
    descriptorImageInfos[index]
        .setSampler(mSamplers[index])
        .setImageView(mTextures[index]->getImageView())
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
  }

  vk::WriteDescriptorSet descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(descriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
          .setImageInfo(descriptorImageInfos);

  Context::get().getDevice().getLogicalDeviceHandle().updateDescriptorSets(
      descriptorWrite, nullptr);
}

ShaderParameterTextureArray::~ShaderParameterTextureArray() {
  for (vk::Sampler& sampler : mSamplers) {
    Context::get().getDevice().getLogicalDeviceHandle().destroySampler(sampler);
  }
}
