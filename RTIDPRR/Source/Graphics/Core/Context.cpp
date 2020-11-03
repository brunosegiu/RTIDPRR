#include "Context.h"

using namespace RTIDPRR::Graphics;

Context::Context(const SDL_Window* window) : mInstance(window), mDevice(mInstance), mSwapchain(window, mInstance, mDevice) {
}

Context::~Context() {}
