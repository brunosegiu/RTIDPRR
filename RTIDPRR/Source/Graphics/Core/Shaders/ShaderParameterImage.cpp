#include "ShaderParameterImage.h"

#include "../../../Misc/DebugUtils.h"
#include "../Context.h"

using namespace RTIDPRR::Graphics;

ShaderParameterImage::ShaderParameterImage(const Texture* texture)
    : mTexture(texture) {}

ShaderParameterImage::ShaderParameterImage(
    ShaderParameterImage&& other) noexcept
    : mTexture(std::move(other.mTexture)) {}

void ShaderParameterImage::bindToGroup(const vk::DescriptorSet& descriptorSet,
                                       const uint32_t binding) {
  vk::DescriptorImageInfo descriptorImageInfo =
      vk::DescriptorImageInfo()
          .setImageView(mTexture->getImageView())
          .setImageLayout(vk::ImageLayout::eGeneral);

  vk::WriteDescriptorSet descriptorWrite =
      vk::WriteDescriptorSet()
          .setDstSet(descriptorSet)
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDescriptorType(getDescriptorType())
          .setImageInfo(descriptorImageInfo);

  Context::get().getDevice().getLogicalDeviceHandle().updateDescriptorSets(
      descriptorWrite, nullptr);
}

ShaderParameterImage::~ShaderParameterImage() {}
