#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
	namespace Graphics {
		class Instance {
		public:
			Instance(const SDL_Window* window);

			const vk::SurfaceKHR initSurface(const SDL_Window* window);

			const vk::Instance& getHandle() const { return mInstanceHandle; };

			virtual ~Instance();

		private:
			Instance(Instance const&) = delete;
			Instance& operator=(Instance const&) = delete;

			static const std::vector<const char*> getRequiredExtensions(const SDL_Window* window);

			vk::Instance mInstanceHandle;

#if defined(_DEBUG)
			vk::DebugUtilsMessengerEXT mDebugMessenger;
#endif
		};
	} // namespace Graphics
} // namespace RTIDPRR
