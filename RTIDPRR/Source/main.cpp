// Enable the WSI extensions

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Graphics/Core/Context.h"
#include "Graphics/DeferredRendering/DeferredRenderer.h"

int main() {
  assert(SDL_Init(SDL_INIT_VIDEO) == 0);

  SDL_Window* window =
      SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_VULKAN);
  assert(window);

  RTIDPRR::Graphics::Context::init(window);
  RTIDPRR::Graphics::DeferredRenderer renderer;

  // Poll for user input.
  bool stillRunning = true;
  while (stillRunning) {
    renderer.render();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          stillRunning = false;
          break;

        default:
          // Do nothing.
          break;
      }
    }

    SDL_Delay(10);
  }

  // Clean up.
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
