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

  const vk::ImageView& getImageView() const { return mImageView; }

  virtual ~Texture();

 private:
  vk::Image mImage;
  vk::ImageView mImageView;
  vk::Format mFormat;
  vk::DeviceMemory mMemory;
};
}  // namespace Graphics
}  // namespace RTIDPRR
