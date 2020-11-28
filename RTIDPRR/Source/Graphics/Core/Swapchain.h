#pragma once

#include <vulkan/vulkan.hpp>

#include "../../Misc/Window.h"
#include "Device.h"
#include "Framebuffer.h"

namespace RTIDPRR {
namespace Graphics {

struct SwapchainResources {
  vk::Image mImage;
  vk::ImageView mImageView;
  Framebuffer mFramebuffer;
};

class Swapchain {
 public:
  Swapchain(const Window& window, const Instance& instance,
            const Device& device);

  const Framebuffer& getCurrentFramebuffer() const {
    return mSwapchainResources[mCurrentImageIndex].mFramebuffer;
  }
  const vk::RenderPass& getMainRenderPass() const { return mMainRenderPass; };

  void swapBuffers();
  void submitCommand(const vk::CommandBuffer& commandBuffer);

  virtual ~Swapchain();

 private:
  vk::SurfaceKHR mWindowSurface;
  vk::SwapchainKHR mSwapchainHandle;
  std::vector<SwapchainResources> mSwapchainResources;
  vk::RenderPass mMainRenderPass;

  vk::Semaphore mImageAvailableSemaphore;
  vk::Semaphore mPresentFinishedSemaphore;

  uint32_t mCurrentImageIndex;
};
}  // namespace Graphics
}  // namespace RTIDPRR
