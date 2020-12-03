#include "Swapchain.h"

#include <algorithm>
#include <cstdint>
#include <limits>

#include "../../Misc/DebugUtils.h"
#include "Context.h"

using namespace RTIDPRR::Graphics;

Swapchain::Swapchain(const Window& window, const Instance& instance,
                     const Device& device)
    : mWindowSurface(window.createSurface(instance.getHandle())),
      mCurrentImageIndex(0),
      mDevice(device),
      mInstance(instance) {
  const std::vector<vk::PresentModeKHR> presentModes =
      device.getPhysicalDeviceHandle().getSurfacePresentModesKHR(
          mWindowSurface);
  const vk::PresentModeKHR presentMode =
      std::find(presentModes.begin(), presentModes.end(),
                vk::PresentModeKHR::eFifo) != presentModes.end()
          ? vk::PresentModeKHR::eFifo
          : vk::PresentModeKHR::eMailbox;
  const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      device.getPhysicalDeviceHandle().getSurfaceCapabilitiesKHR(
          mWindowSurface);

  mSwapchainExtent = surfaceCapabilities.currentExtent;
  uint32_t imageCount = std::clamp<uint32_t>(
      surfaceCapabilities.minImageCount + 1, surfaceCapabilities.minImageCount,
      surfaceCapabilities.maxImageCount);

  const std::vector<vk::SurfaceFormatKHR> surfaceFormats =
      device.getPhysicalDeviceHandle().getSurfaceFormatsKHR(mWindowSurface);
  const vk::Format surfaceFormat = vk::Format::eB8G8R8A8Srgb;
  const bool supportedFormat =
      std::find_if(
          surfaceFormats.begin(), surfaceFormats.end(),
          [&surfaceFormat](const vk::SurfaceFormatKHR& supportedFormat) {
            return surfaceFormat == supportedFormat.format;
          }) != surfaceFormats.end();
  RTIDPRR_ASSERT_MSG(supportedFormat,
                     "B8G8R8A8Srgb format not supported by platform");
  RTIDPRR_ASSERT_MSG(
      device.getPhysicalDeviceHandle().getSurfaceSupportKHR(
          device.getGraphicsQueue().getFamilyIndex(), mWindowSurface),
      "Cannot present using the graphics queue");

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

  mMainRenderPass =
      device.getLogicalDeviceHandle().createRenderPass(renderPassCreateInfo);

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
          .setClipped(true);

  mSwapchainHandle =
      device.getLogicalDeviceHandle().createSwapchainKHR(swapchainCreateInfo);
  std::vector<vk::Image> images =
      device.getLogicalDeviceHandle().getSwapchainImagesKHR(mSwapchainHandle);

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
        device.getLogicalDeviceHandle().createImageView(
            swapchainImageViewCreateInfo);
    Framebuffer swapchainImageFramebuffer(
        device, mMainRenderPass, {swapchainImageView}, window.getWidth(),
        window.getHeight());
    SwapchainResources resources{std::move(swapchainImage),
                                 std::move(swapchainImageView),
                                 std::move(swapchainImageFramebuffer)};
    mSwapchainResources.emplace_back(std::move(resources));
  }

  vk::SemaphoreCreateInfo semaphoreCreateInfo;
  mImageAvailableSemaphore =
      device.getLogicalDeviceHandle().createSemaphore(semaphoreCreateInfo);
  mPresentFinishedSemaphore =
      device.getLogicalDeviceHandle().createSemaphore(semaphoreCreateInfo);
}

void Swapchain::swapBuffers() {
  const Device& device = Context::get().getDevice();

  mCurrentImageIndex =
      device.getLogicalDeviceHandle()
          .acquireNextImageKHR(mSwapchainHandle,
                               std::numeric_limits<uint64_t>::max(),
                               mImageAvailableSemaphore, nullptr)
          .value;
}

void Swapchain::submitCommand(const vk::CommandBuffer& commandBuffer) {
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
  queue.submit(submitInfo);

  vk::PresentInfoKHR presentInfo =
      vk::PresentInfoKHR()
          .setWaitSemaphores(mPresentFinishedSemaphore)
          .setSwapchains(mSwapchainHandle)
          .setImageIndices(mCurrentImageIndex);

  queue.present(presentInfo);
}

Swapchain::~Swapchain() {
  mDevice.getLogicalDeviceHandle().destroySwapchainKHR(mSwapchainHandle);
  for (SwapchainResources& resource : mSwapchainResources) {
    mDevice.getLogicalDeviceHandle().destroyImageView(resource.mImageView);
    // Framebuffer destroyed by Framebuffer::~Framebuffer
    // Image destroyed by swapchain
  }
  mDevice.getLogicalDeviceHandle().destroyRenderPass(mMainRenderPass);
  mDevice.getLogicalDeviceHandle().destroySemaphore(mImageAvailableSemaphore);
  mDevice.getLogicalDeviceHandle().destroySemaphore(mPresentFinishedSemaphore);
  mInstance.getHandle().destroySurfaceKHR(mWindowSurface);
}
