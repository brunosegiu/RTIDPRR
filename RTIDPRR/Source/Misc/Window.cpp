#include "Window.h"

#include "DebugUtils.h"

using namespace RTIDPRR;

static const char* APP_NAME = "RTIDPRR";

Window::Window(uint32_t width, uint32_t height)
    : mWindow(nullptr), mWidth(width), mHeight(height) {
  RTIDPRR_ASSERT(SDL_Init(SDL_INIT_VIDEO) == 0);
  mWindow =
      SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_VULKAN);
  RTIDPRR_ASSERT(mWindow);
}

bool Window::processInput() const {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
        break;
      default:
        break;
    }
  }
  return true;
}

vk::SurfaceKHR Window::createSurface(const vk::Instance& instance) const {
  VkSurfaceKHR surface{};
  RTIDPRR_ASSERT_MSG(
      SDL_Vulkan_CreateSurface(const_cast<SDL_Window*>(mWindow),
                               static_cast<VkInstance>(instance), &surface),
      "Failed creating main window surface");
  return vk::SurfaceKHR(surface);
}

std::vector<const char*> Window::getRequiredVkExtensions() const {
  unsigned int requiredExtensionsCount = 0;
  RTIDPRR_ASSERT(SDL_Vulkan_GetInstanceExtensions(
      const_cast<SDL_Window*>(mWindow), &requiredExtensionsCount, NULL));
  std::vector<const char*> extensions(requiredExtensionsCount);
  RTIDPRR_ASSERT(SDL_Vulkan_GetInstanceExtensions(
      const_cast<SDL_Window*>(mWindow), &requiredExtensionsCount,
      extensions.data()));
  return extensions;
}

Window::~Window() {
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}
