#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Instance.h"
#include "Device.h"
#include "Swapchain.h"

#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

namespace RTIDPRR {
	namespace Graphics {
		class Context {
		public:
			Context(const SDL_Window* window);

			const Device& getDevice() const { return mDevice; }

			virtual ~Context();

		private:
			Context(Context const&) = delete;
			Context& operator=(Context const&) = delete;

			Instance mInstance;
			Device mDevice;
			Swapchain mSwapchain;
		};
	} // namespace Graphics
} // namespace RTIDPRR
