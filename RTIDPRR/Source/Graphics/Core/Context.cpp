#include "Context.h"

using namespace RTIDPRR::Graphics;

std::unique_ptr<Context> Context::sContext = nullptr;

const Context& Context::init(const SDL_Window* window) {
  assert(!sContext);
  if (!sContext) {
    sContext = std::make_unique<Context>(window);
  }
  return *sContext;
}

Context& Context::get() {
  assert(sContext);
  return *sContext;
}

Context::Context(const SDL_Window* window)
    : mInstance(window),
      mDevice(mInstance),
      mSwapchain(window, mInstance, mDevice) {}

Context::~Context() {}
