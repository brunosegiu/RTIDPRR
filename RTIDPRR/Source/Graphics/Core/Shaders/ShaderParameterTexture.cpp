#include "ShaderParameterTexture.h"

#include "../../../Misc/DebugUtils.h"
#include "../Context.h"

using namespace RTIDPRR::Graphics;

ShaderParameterTexture::ShaderParameterTexture(
    const SamplerOptions& samplerOptions)
    : mTexture(samplerOptions.texture) {
  const Device& device = Context::get().getDevice();
  vk::SamplerCreateInfo samplerCreateInfo =
      vk::SamplerCreateInfo()
          .setMagFilter(samplerOptions.filter)
          .setMinFilter(samplerOptions.filter)
          .setMipmapMode(vk::SamplerMipmapMode::eLinear)
          .setAddressModeU(samplerOptions.clampMode)
          .setAddressModeV(samplerOptions.clampMode)
          .setAddressModeW(samplerOptions.clampMode)
          .setMipLodBias(0.0f)
          .setMaxAnisotropy(1.0f)
          .setMinLod(0.0f)
          .setMaxLod(1.0f)
          .setBorderColor(samplerOptions.borderColor);
  mSampler = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createSampler(samplerCreateInfo));
}

ShaderParameterTexture::ShaderParameterTexture(ShaderParameterTexture&& other)
    : mSampler(std::move(other.mSampler)), mTexture(std::move(other.mTexture)) {
  other.mSampler = nullptr;
}

void ShaderParameterTexture::bindToGroup(const vk::DescriptorSet& descriptorSet,
                                         const uint32_t binding) {
  vk::DescriptorImageInfo descriptorImageInfo =
      vk::DescriptorImageInfo()
          .setSampler(mSampler)
          .setImageView(mTexture->getImageView())
          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

  vk::WriteDescriptorSet descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(descriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
          .setImageInfo(descriptorImageInfo);

  Context::get().getDevice().getLogicalDeviceHandle().updateDescriptorSets(
      descriptorWrite, nullptr);
}

ShaderParameterTexture::~ShaderParameterTexture() {
  Context::get().getDevice().getLogicalDeviceHandle().destroySampler(mSampler);
}
