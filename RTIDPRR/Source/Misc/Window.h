#pragma once

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

#include "../Input/InputController.h"

namespace RTIDPRR {
namespace Input {
class InputController;
}  // namespace Input
}  // namespace RTIDPRR

namespace RTIDPRR {
class Window {
 public:
  inline static Window* sInstance = nullptr;

  static Window& init(uint32_t width, uint32_t height) {
    sInstance = new Window(width, height);
    return *sInstance;
  };
  static Window& get() { return *sInstance; };

  Window(uint32_t width, uint32_t height);

  bool processInput(float timeDelta);

  vk::SurfaceKHR createSurface(const vk::Instance& instance) const;
  std::vector<const char*> getRequiredVkExtensions() const;

  uint32_t getWidth() const { return mWidth; };
  uint32_t getHeight() const { return mHeight; };
  RTIDPRR::Input::InputController& getInputController();

  void warpCursorToCenter();
  void toggleCursor(bool show);

  virtual ~Window();

 private:
  SDL_Window* mWindow;
  uint32_t mWidth, mHeight;
  RTIDPRR::Input::InputController mInputController;
};
}  // namespace RTIDPRR
