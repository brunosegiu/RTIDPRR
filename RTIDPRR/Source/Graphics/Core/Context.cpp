#include "Context.h"

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

std::unique_ptr<Context> Context::sContext = nullptr;

const Context& Context::init(const Window& window) {
  RTIDPRR_ASSERT_MSG(!sContext, "Graphics context initialized twice!");
  if (!sContext) {
    sContext = std::make_unique<Context>(window);
  }
  return *sContext;
}

void Context::terminate() { sContext.reset(); }

Context& Context::get() {
  RTIDPRR_ASSERT_MSG(sContext, "Graphics context not yet initialized!");
  return *sContext;
}

Context::Context(const Window& window)
    : mInstance(window),
      mDevice(mInstance),
      mSwapchain(window, mInstance, mDevice),
      mCommandPool(mDevice.getLogicalDeviceHandle(),
                   mDevice.getGraphicsQueue().getFamilyIndex()) {}

Context::~Context() {}
