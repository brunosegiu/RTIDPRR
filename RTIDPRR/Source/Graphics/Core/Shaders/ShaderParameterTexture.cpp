#include "ShaderParameterTexture.h"

#include "../Context.h"

using namespace RTIDPRR::Graphics;

ShaderParameterTexture::ShaderParameterTexture(const Texture& texture)
    : mTexture(texture) {
  const Device& device = Context::get().getDevice();
  vk::SamplerCreateInfo samplerCreateInfo =
      vk::SamplerCreateInfo()
          .setMagFilter(vk::Filter::eNearest)
          .setMinFilter(vk::Filter::eNearest)
          .setMipmapMode(vk::SamplerMipmapMode::eLinear)
          .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
          .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
          .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
          .setMipLodBias(0.0f)
          .setMaxAnisotropy(1.0f)
          .setMinLod(0.0f)
          .setMaxLod(1.0f)
          .setBorderColor(vk::BorderColor::eFloatOpaqueWhite);
  mSampler = device.getLogicalDeviceHandle().createSampler(samplerCreateInfo);
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
          .setImageView(mTexture.getImageView())
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
