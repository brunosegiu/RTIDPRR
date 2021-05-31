#pragma once

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {
class Texture {
 public:
  Texture(const vk::Extent2D& extent, const vk::Format& format,
          const vk::ImageUsageFlags& usage,
          const vk::ImageAspectFlags& viewAspect,
          const vk::ImageTiling tiling = vk::ImageTiling::eOptimal);

  Texture(Texture&& other) noexcept
      : mImage(std::move(other.mImage)),
        mImageView(std::move(other.mImageView)),
        mFormat(std::move(other.mFormat)),
        mMemory(std::move(other.mMemory)),
        mExtent(std::move(other.mExtent)),
        mUsage(std::move(other.mUsage)) {
    other.mImage = nullptr;
    other.mImageView = nullptr;
    other.mMemory = nullptr;
  }

  const vk::Image& getImage() const { return mImage; }
  const vk::ImageView& getImageView() const { return mImageView; }
  const vk::Format& getFormat() const { return mFormat; }
  const vk::Extent2D& getExtent() const { return mExtent; }
  const vk::ImageUsageFlags& getUsage() const { return mUsage; }
  virtual ~Texture();

 private:
  vk::Image mImage;
  vk::ImageView mImageView;
  vk::Format mFormat;
  vk::DeviceMemory mMemory;
  vk::Extent2D mExtent;
  vk::ImageUsageFlags mUsage;
};
}  // namespace Graphics
}  // namespace RTIDPRR
