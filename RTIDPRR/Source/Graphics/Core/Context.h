#pragma once

#include <memory>

#include "../../Misc/Window.h"
#include "Device.h"
#include "Instance.h"
#include "Swapchain.h"

#define VK_USE_PLATFORM_WIN32_KHR

namespace RTIDPRR {
namespace Graphics {
class Context {
 public:
  static const Context& init(const Window& window);
  static void terminate();
  static Context& get();

  Context(const Window& window);

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
