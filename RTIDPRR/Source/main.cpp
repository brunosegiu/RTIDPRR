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
  newObject.addComponent<Mesh>("Assets/Models/sphere.glb");

  Object& newObject1 = scene.addObject();
  newObject1.addComponent<Mesh>("Assets/Models/monkey.glb");
  newObject1.getComponent<Transform>()->translate(glm::vec3(10, 0, 5));

  Object& newObject2 = scene.addObject();
  newObject2.addComponent<Mesh>("Assets/Models/cube.glb");
  newObject2.getComponent<Transform>()->translate(glm::vec3(15, 0, 0));
  newObject2.getComponent<Transform>()->scale(glm::vec3(10, 0.3, 10));
  newObject2.getComponent<Transform>()->rotate(
      glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(-90.0f)));

  Object& light = scene.addObject();
  light.addComponent<Light>();
  light.getComponent<Light>()->setIntensity(0.1f);
  light.getComponent<Transform>()->translate(glm::vec3(-5.0f, 0.0f, 0.0f));

  Object& light2 = scene.addObject();
  light2.addComponent<Light>()->setIntensity(0.1f);
  light2.getComponent<Transform>()->rotate(0.2f * glm::vec3(0, 1, 0));
  light2.getComponent<Transform>()->translate(glm::vec3(-5.2f, 0.0f, 0.05f));

  bool open = true;
  RTIDPRR::Time::Timer timer;
  float deltaTime = 0.0f;
  do {
    timer.restart();
    open = !window.processInput(deltaTime);

    // Update test object
    newObject1.getComponent<Transform>()->rotate((deltaTime * 0.000001f) *
                                                 glm::vec3(0, 1, 0));
    // Update scene, systems and render
    scene.update(deltaTime);
    renderer.render(scene);
    timer.end();
    deltaTime = timer.getDeltaMs();
  } while (open);
  return 0;
}
