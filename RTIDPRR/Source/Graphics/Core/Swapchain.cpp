#include "Swapchain.h"

#include <algorithm>
#include <cstdint>
#include <limits>

#include "../../Misc/DebugUtils.h"
#include "Command.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

Swapchain::Swapchain(const Window& window, const Instance& instance,
                     const Device& device)
    : mWindowSurface(window.createSurface(instance.getHandle())),
      mCurrentImageIndex(0),
      mDevice(device),
      mInstance(instance) {
  createResources(device);
}

void Swapchain::swapBuffers() {
  const Device& device = Context::get().getDevice();

  mCurrentImageIndex =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().acquireNextImageKHR(
          mSwapchainHandle, std::numeric_limits<uint64_t>::max(),
          mImageAvailableSemaphore, nullptr));
}

void Swapchain::present(const Command& commandBuffer) {
  const std::vector<vk::PipelineStageFlags> waitStages{
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  const std::vector<vk::CommandBuffer> commands{commandBuffer};
  vk::SubmitInfo submitInfo =
      vk::SubmitInfo()
          .setWaitSemaphores(mImageAvailableSemaphore)
          .setWaitDstStageMask(waitStages)
          .setCommandBuffers(commands)
          .setSignalSemaphores(mPresentFinishedSemaphore);
  const Queue& queue = Context::get().getDevice().getGraphicsQueue();
  queue.submit(submitInfo, mFrameCommandsDone);

  vk::PresentInfoKHR presentInfo =
      vk::PresentInfoKHR()
          .setWaitSemaphores(mPresentFinishedSemaphore)
          .setSwapchains(mSwapchainHandle)
          .setImageIndices(mCurrentImageIndex);

  vk::Result presentResult = queue.present(presentInfo);

  const Device& device = Context::get().getDevice();
  RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().waitForFences(
      mFrameCommandsDone, true, std::numeric_limits<uint64_t>::max()));
  device.getLogicalDeviceHandle().resetFences(mFrameCommandsDone);

  if (presentResult == vk::Result::eErrorOutOfDateKHR ||
      presentResult == vk::Result::eSuboptimalKHR) {
    cleanupResources();
    createResources(Context::get().getDevice());
  }
}

Swapchain::~Swapchain() {
  mDevice.getLogicalDeviceHandle().destroySwapchainKHR(mSwapchainHandle);
  for (SwapchainResources& resource : mSwapchainResources) {
    mDevice.getLogicalDeviceHandle().destroyImageView(resource.mImageView);
    // Framebuffer destroyed by Framebuffer::~Framebuffer
    // Image destroyed by swapchain
  }
  mDevice.getLogicalDeviceHandle().destroySemaphore(mImageAvailableSemaphore);
  mDevice.getLogicalDeviceHandle().destroySemaphore(mPresentFinishedSemaphore);
  mDevice.getLogicalDeviceHandle().destroyFence(mFrameCommandsDone);
  mInstance.getHandle().destroySurfaceKHR(mWindowSurface);
}

void Swapchain::createResources(const Device& device) {
  mCurrentImageIndex = 0;
  const std::vector<vk::PresentModeKHR> presentModes = RTIDPRR_ASSERT_VK(
      device.getPhysicalDeviceHandle().getSurfacePresentModesKHR(
          mWindowSurface));
  const vk::PresentModeKHR presentMode =
      std::find(presentModes.begin(), presentModes.end(),
                vk::PresentModeKHR::eFifo) != presentModes.end()
          ? vk::PresentModeKHR::eFifo
          : vk::PresentModeKHR::eMailbox;
  const vk::SurfaceCapabilitiesKHR surfaceCapabilities = RTIDPRR_ASSERT_VK(
      device.getPhysicalDeviceHandle().getSurfaceCapabilitiesKHR(
          mWindowSurface));

  mSwapchainExtent = surfaceCapabilities.currentExtent;
  uint32_t imageCount = std::clamp<uint32_t>(
      surfaceCapabilities.minImageCount + 1, surfaceCapabilities.minImageCount,
      surfaceCapabilities.maxImageCount);

  const std::vector<vk::SurfaceFormatKHR> surfaceFormats = RTIDPRR_ASSERT_VK(
      device.getPhysicalDeviceHandle().getSurfaceFormatsKHR(mWindowSurface));
  const vk::Format surfaceFormat = vk::Format::eB8G8R8A8Srgb;
  const bool supportedFormat =
      std::find_if(
          surfaceFormats.begin(), surfaceFormats.end(),
          [&surfaceFormat](const vk::SurfaceFormatKHR& supportedFormat) {
            return surfaceFormat == supportedFormat.format;
          }) != surfaceFormats.end();
  RTIDPRR_ASSERT_MSG(supportedFormat,
                     "B8G8R8A8Srgb format not supported by platform");
  RTIDPRR_ASSERT_VK(device.getPhysicalDeviceHandle().getSurfaceSupportKHR(
      device.getGraphicsQueue().getFamilyIndex(), mWindowSurface));

  // Initialize main render pass
  vk::AttachmentDescription colorAttachmentDescription =
      vk::AttachmentDescription()
          .setFormat(surfaceFormat)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colorAttachmentRef =
      vk::AttachmentReference().setAttachment(0).setLayout(
          vk::ImageLayout::eColorAttachmentOptimal);

  vk::SubpassDescription subpass =
      vk::SubpassDescription().setColorAttachments(colorAttachmentRef);

  vk::RenderPassCreateInfo renderPassCreateInfo =
      vk::RenderPassCreateInfo()
          .setAttachments(colorAttachmentDescription)
          .setSubpasses(subpass);

  mMainRenderPass = std::make_unique<RenderPass>(
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createRenderPass(
          renderPassCreateInfo)),
      1, false);

  vk::SwapchainCreateInfoKHR swapchainCreateInfo =
      vk::SwapchainCreateInfoKHR()
          .setSurface(mWindowSurface)
          .setMinImageCount(imageCount)
          .setImageFormat(surfaceFormat)
          .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
          .setImageExtent(mSwapchainExtent)
          .setImageArrayLayers(1)
          .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
          .setImageSharingMode(vk::SharingMode::eExclusive)
          .setPreTransform(surfaceCapabilities.currentTransform)
          .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
          .setPresentMode(presentMode)
          .setClipped(true)
          .setOldSwapchain(mSwapchainHandle);

  mSwapchainHandle = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createSwapchainKHR(swapchainCreateInfo));
  std::vector<vk::Image> images = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().getSwapchainImagesKHR(mSwapchainHandle));

  mSwapchainResources.clear();
  mSwapchainResources.reserve(images.size());

  for (const vk::Image& swapchainImage : images) {
    vk::ImageViewCreateInfo swapchainImageViewCreateInfo =
        vk::ImageViewCreateInfo()
            .setImage(swapchainImage)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(surfaceFormat)
            .setSubresourceRange(
                vk::ImageSubresourceRange{}
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setLevelCount(1)
                    .setLayerCount(1));
    vk::ImageView swapchainImageView =
        RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createImageView(
            swapchainImageViewCreateInfo));
    Framebuffer swapchainImageFramebuffer(
        device, mMainRenderPass->getHandle(), {swapchainImageView},
        mSwapchainExtent.width, mSwapchainExtent.height);
    SwapchainResources resources{std::move(swapchainImage),
                                 std::move(swapchainImageView),
                                 std::move(swapchainImageFramebuffer)};
    mSwapchainResources.emplace_back(std::move(resources));
  }

  vk::SemaphoreCreateInfo semaphoreCreateInfo;
  mImageAvailableSemaphore = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createSemaphore(semaphoreCreateInfo));
  mPresentFinishedSemaphore = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createSemaphore(semaphoreCreateInfo));

  vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo();
  mFrameCommandsDone = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().createFence(fenceCreateInfo));
}

void Swapchain::cleanupResources() {
  RTIDPRR_ASSERT_VK(
      Context::get().getDevice().getLogicalDeviceHandle().waitIdle());
  for (SwapchainResources& resource : mSwapchainResources) {
    mDevice.getLogicalDeviceHandle().destroyImageView(resource.mImageView);
    // Framebuffer destroyed by Framebuffer::~Framebuffer
    // Image destroyed by swapchain
  }
  mDevice.getLogicalDeviceHandle().destroySemaphore(mImageAvailableSemaphore);
  mDevice.getLogicalDeviceHandle().destroySemaphore(mPresentFinishedSemaphore);
  mDevice.getLogicalDeviceHandle().destroyFence(mFrameCommandsDone);
}
