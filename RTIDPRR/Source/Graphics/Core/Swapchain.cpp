#include "Swapchain.h"

#include <algorithm>

using namespace RTIDPRR::Graphics;

Swapchain::Swapchain(const SDL_Window* window, const Instance& instance, const Device& device) : mWindowSurface(instance.initSurface(window)){
	const std::vector<vk::PresentModeKHR> presentModes = device.getPhysicalDevice().getSurfacePresentModesKHR(mWindowSurface);
	const vk::PresentModeKHR presentMode = std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eFifo) != presentModes.end() ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eMailbox;
	const vk::SurfaceCapabilitiesKHR surfaceCapabilities = device.getPhysicalDevice().getSurfaceCapabilitiesKHR(mWindowSurface);
	
    vk::Extent2D swapchainExtent = surfaceCapabilities.currentExtent;
	uint32_t imageCount = std::clamp<uint32_t>(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

	const vk::Format surfaceFormat = vk::Format::eB8G8R8A8Srgb;

	vk::SwapchainCreateInfoKHR swapchainCreateInfo =
		vk::SwapchainCreateInfoKHR()
		.setSurface(mWindowSurface)
		.setMinImageCount(imageCount)
		.setImageFormat(surfaceFormat)
		.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
		.setImageExtent(swapchainExtent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setPreTransform(surfaceCapabilities.currentTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(presentMode)
		.setClipped(true);

	mSwapchainHandle = device.getLogicalDevice().createSwapchainKHR(swapchainCreateInfo);
	mSwapchainImages = device.getLogicalDevice().getSwapchainImagesKHR(mSwapchainHandle);

	mSwapchainImageViews.reserve(mSwapchainImages.size());
	for (const vk::Image& swapchainImage : mSwapchainImages) {
		vk::ImageViewCreateInfo swapchainImageViewCreateInfo =
			vk::ImageViewCreateInfo()
			.setImage(swapchainImage)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(surfaceFormat);
		mSwapchainImageViews.emplace_back(
			device.getLogicalDevice().createImageView(swapchainImageViewCreateInfo)
		);
	}
}

Swapchain::~Swapchain() {

}