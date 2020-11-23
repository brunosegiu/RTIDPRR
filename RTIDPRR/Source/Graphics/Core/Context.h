#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <memory>

#include "Device.h"
#include "Instance.h"
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
  static const Context& init(const SDL_Window* window);
  static Context& get();

  Context(const SDL_Window* window);

  const Device& getDevice() const { return mDevice; }
  const Swapchain& getSwapchain() const { return mSwapchain; };
  Swapchain& getSwapchain() { return mSwapchain; };

  virtual ~Context();

 private:
  Context(Context const&) = delete;
  Context& operator=(Context const&) = delete;

  static std::unique_ptr<Context> sContext;

  Instance mInstance;
  Device mDevice;
  Swapchain mSwapchain;

  vk::CommandPool mDefaultCommandPool;
};
}  // namespace Graphics
}  // namespace RTIDPRR
