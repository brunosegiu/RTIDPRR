#pragma once

#include <vulkan/vulkan.hpp>

#include "../../Misc/Window.h"
#include "Device.h"
#include "Framebuffer.h"
#include "RenderPass.h"

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
            const Device& logicalDevice);

  const Framebuffer& getCurrentFramebuffer() const {
    return mSwapchainResources[mCurrentImageIndex].mFramebuffer;
  }
  const RenderPass& getMainRenderPass() const { return *mMainRenderPass; };
  const vk::Extent2D& getExtent() const { return mSwapchainExtent; };

  void swapBuffers();
  void submitCommand(const vk::CommandBuffer& commandBuffer);

  virtual ~Swapchain();

 private:
  vk::SurfaceKHR mWindowSurface;
  vk::SwapchainKHR mSwapchainHandle;
  std::vector<SwapchainResources> mSwapchainResources;
  std::unique_ptr<RenderPass> mMainRenderPass;

  vk::Semaphore mImageAvailableSemaphore;
  vk::Semaphore mPresentFinishedSemaphore;

  uint32_t mCurrentImageIndex;

  vk::Extent2D mSwapchainExtent;

  const Device& mDevice;
  const Instance& mInstance;

  void createResources(const Device& device);
  void cleanupResources();
};
}  // namespace Graphics
}  // namespace RTIDPRR
