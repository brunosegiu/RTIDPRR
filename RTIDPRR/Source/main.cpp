#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Graphics/Core/Context.h"
#include "Graphics/DeferredRendering/DeferredRenderer.h"
#include "Input/CameraController.h"
#include "Misc/Time/Timer.h"
#include "Misc/Window.h"

int main() {
  RTIDPRR::Window& window = RTIDPRR::Window::init(1280, 720);
  RTIDPRR::Graphics::Context::init(window);
  RTIDPRR::Graphics::DeferredRenderer renderer;
  RTIDPRR::Graphics::Scene scene;

  RTIDPRR::Input::CameraController cameraController(scene.getCamera());

  window.getInputController().attach(&cameraController);

  bool open = true;
  RTIDPRR::Time::Timer timer;
  float timeDelta = 0.0f;
  do {
    timer.restart();
    open = !window.processInput(timeDelta);
    renderer.render(scene);
    timer.end();
    timeDelta = timer.getDeltaMs();
  } while (open);
  return 0;
}
