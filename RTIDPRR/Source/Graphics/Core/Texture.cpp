#include "Texture.h"

#include "Context.h"
#include "DeviceMemory.h"

using namespace RTIDPRR::Graphics;

Texture::Texture(const vk::Extent2D& extent, const vk::Format& format,
                 const vk::ImageUsageFlags& usage,
                 const vk::ImageAspectFlags& viewAspect,
                 const vk::ImageTiling tiling)
    : mFormat(format) {
  const Device& device = Context::get().getDevice();
  vk::ImageCreateInfo imageCreateInfo =
      vk::ImageCreateInfo()
          .setImageType(vk::ImageType::e2D)
          .setFormat(format)
          .setExtent({extent.width, extent.height, 1})
          .setMipLevels(1)
          .setArrayLayers(1)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setTiling(tiling)
          .setUsage(usage)
          .setInitialLayout(vk::ImageLayout::eUndefined);
  mImage = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createImage(imageCreateInfo));
  vk::MemoryRequirements memReqs =
      device.getLogicalDeviceHandle().getImageMemoryRequirements(mImage);
  vk::MemoryAllocateInfo memAllocInfo =
      vk::MemoryAllocateInfo()
          .setAllocationSize(memReqs.size)
          .setMemoryTypeIndex(DeviceMemory::findMemoryIndex(
              memReqs.memoryTypeBits,
              vk::MemoryPropertyFlagBits::eDeviceLocal));
  mMemory = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().allocateMemory(memAllocInfo));
  device.getLogicalDeviceHandle().bindImageMemory(mImage, mMemory, 0);

  vk::ImageSubresourceRange subsurfaceRange = vk::ImageSubresourceRange()
                                                  .setAspectMask(viewAspect)
                                                  .setBaseMipLevel(0)
                                                  .setLevelCount(1)
                                                  .setBaseArrayLayer(0)
                                                  .setLayerCount(1);
  vk::ImageViewCreateInfo imageViewCreateInfo =
      vk::ImageViewCreateInfo{}
          .setViewType(vk::ImageViewType::e2D)
          .setFormat(format)
          .setSubresourceRange(subsurfaceRange)
          .setImage(mImage);
  mImageView = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createImageView(imageViewCreateInfo));
}

Texture::~Texture() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().destroyImage(mImage);
  device.getLogicalDeviceHandle().destroyImageView(mImageView);
  device.getLogicalDeviceHandle().freeMemory(mMemory);
}
