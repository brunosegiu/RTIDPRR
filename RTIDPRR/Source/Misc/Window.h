#pragma once

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
class Window {
 public:
  Window(uint32_t width, uint32_t height);

  bool processInput() const;

  vk::SurfaceKHR createSurface(const vk::Instance& instance) const;
  std::vector<const char*> getRequiredVkExtensions() const;

  uint32_t getWidth() const { return mWidth; };
  uint32_t getHeight() const { return mHeight; };

  virtual ~Window();

 private:
  SDL_Window* mWindow;
  uint32_t mWidth, mHeight;
};
}  // namespace RTIDPRR
