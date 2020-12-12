#include <SDL2/SDL.h>
#undef main
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Components/Mesh/Mesh.h"
#include "Components/Transform/Transform.h"
#include "Core/Scene.h"
#include "Graphics/Core/Context.h"
#include "Graphics/DeferredRendering/DeferredRenderer.h"
#include "Misc/Time/Timer.h"
#include "Misc/Window.h"

int main() {
  using namespace RTIDPRR::Graphics;
  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Input;
  using namespace RTIDPRR::Component;

  RTIDPRR::Window& window = RTIDPRR::Window::init(1378, 768);
  Context::init(window);
  DeferredRenderer renderer;
  Scene scene;

  // Testing
  Object& newObject = scene.addObject();
  Transform* transform = newObject.getComponent<Transform>();
  Mesh* mesh = newObject.addComponent<Mesh>("Assets/Models/monkey.glb");

  bool open = true;
  RTIDPRR::Time::Timer timer;
  float deltaTime = 0.0f;
  do {
    timer.restart();
    open = !window.processInput(deltaTime);

    // Update test objects
    // transform->rotate((deltaTime * 0.000001f) * glm::vec3(0, 1, 0));

    // Update scene, systems and render
    scene.update(deltaTime);
    renderer.render(scene);
    timer.end();
    deltaTime = timer.getDeltaMs();
  } while (open);
  return 0;
}
