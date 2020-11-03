#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace RTIDPRR {
	namespace Graphics {
		class Swapchain {
		public:
			Swapchain(const SDL_Window* window, const Instance& instance, const Device& device);
			virtual ~Swapchain();

		private:
			vk::SurfaceKHR mWindowSurface;
			vk::SwapchainKHR mSwapchainHandle;
			std::vector<vk::Image> mSwapchainImages;
			std::vector<vk::ImageView> mSwapchainImageViews;
		};
	} // namespace Graphics
} // namespace RTIDPRR
