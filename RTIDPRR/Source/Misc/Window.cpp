#include "Window.h"

#include "DebugUtils.h"

using namespace RTIDPRR;

static const char* APP_NAME = "RTIDPRR";

static int windowEventHandler(void* data, SDL_Event* event) {
  if (event->type == SDL_WINDOWEVENT &&
      event->window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
    if (window == (SDL_Window*)data) {
      int width, height;
      SDL_GetWindowSize(window, &width, &height);
      Window::get().setSize(width, height);
    }
  }
  return 0;
}

Window::Window(uint32_t width, uint32_t height)
    : mWindow(nullptr), mWidth(width), mHeight(height), mInputController(this) {
  RTIDPRR_ASSERT(SDL_Init(SDL_INIT_VIDEO) == 0);
  mWindow =
      SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
  SDL_AddEventWatch(windowEventHandler, mWindow);
  RTIDPRR_ASSERT(mWindow);
}

bool Window::processInput(float timeDelta) {
  return mInputController.processEvents(timeDelta);
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
  uint32_t requiredExtensionsCount = 0;
  RTIDPRR_ASSERT(SDL_Vulkan_GetInstanceExtensions(
      const_cast<SDL_Window*>(mWindow), &requiredExtensionsCount, NULL));
  std::vector<const char*> extensions(requiredExtensionsCount);
  RTIDPRR_ASSERT(SDL_Vulkan_GetInstanceExtensions(
      const_cast<SDL_Window*>(mWindow), &requiredExtensionsCount,
      extensions.data()));
  return extensions;
}

RTIDPRR::Input::InputController& Window::getInputController() {
  return mInputController;
}

void Window::warpCursorToCenter() {
  //SDL_WarpMouseInWindow(mWindow, mWidth / 2, mHeight / 2);
}

void Window::toggleCursor(bool show) {
  //SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void Window::setSize(uint32_t width, uint32_t height) {
  mWidth = width;
  mHeight = height;
}

Window::~Window() {
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}
