#include <SDL2/SDL.h>
#undef main
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Components/Light/Light.h"
#include "Components/Mesh/Mesh.h"
#include "Components/Transform/Transform.h"
#include "Core/Scene.h"
#include "Graphics/Core/Context.h"
#include "Graphics/DeferredRendering/DeferredRenderer.h"
#include "Loaders/GLTFLoader.h"
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

  Object& light = scene.addObject();
  light.addComponent<Light>();
  light.getComponent<Light>()->setIntensity(0.1f);
  light.getComponent<Transform>()->translate(glm::vec3(-30.0f, 0.0f, 10.0f));

  /*Object& light2 = scene.addObject();
  light2.addComponent<Light>();
  light2.getComponent<Light>()->setIntensity(0.1f);
  light2.getComponent<Transform>()->translate(glm::vec3(10.0f, 0.0f, 40.0f));
  light2.getComponent<Transform>()->rotate(
      glm::vec3(0, glm::radians(90.0f), 0.0f));*/

  RTIDPRR::GLTFLoader loader;
  loader.load(scene, "Assets/Models/scene.glb");

  bool open = true;
  RTIDPRR::Time::Timer timer;
  float deltaTime = 0.0f;
  do {
    timer.restart();
    open = !window.processInput(deltaTime);
    // Update scene, systems and render
    scene.update(deltaTime);
    renderer.render(scene);
    timer.end();
    deltaTime = timer.getDeltaMs();
  } while (open);
  return 0;
}
