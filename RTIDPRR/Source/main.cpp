#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Graphics/Core/Context.h"
#include "Graphics/DeferredRendering/DeferredRenderer.h"
#include "Misc/Window.h"

int main() {
  RTIDPRR::Window window(1280, 720);
  RTIDPRR::Graphics::Context::init(window);
  RTIDPRR::Graphics::DeferredRenderer renderer;
  bool open = true;
  do {
    renderer.render();
    open = window.processInput();
    SDL_Delay(10);
  } while (open);
  return 0;
}
