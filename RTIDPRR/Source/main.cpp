#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Components/Mesh/Mesh.h"
#include "Components/Transform/Transform.h"
#include "Core/Scene.h"
#include "Graphics/Core/Context.h"
#include "Graphics/DeferredRendering/DeferredRenderer.h"
#include "Input/CameraController.h"
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

  CameraController cameraController(scene.getCamera());

  window.getInputController().attach(&cameraController);

  // Testing
  Object& newObject = scene.addObject();
  Transform& transform = newObject.addComponent<Transform>();
  Mesh& mesh = newObject.addComponent<Mesh>("Assets/Models/monkey.glb");

  Object& rotatingObject = scene.addObject();
  rotatingObject.addComponent<Mesh>("Assets/Models/monkey.glb");
  Transform& rotTrans = rotatingObject.addComponent<Transform>();
  rotTrans.translate(glm::vec3(3, 0, 3));
  rotTrans.setLocalScale(glm::vec3(0.1f, 0.1f, 0.1f));

  {
    Object& newObject1 = scene.addObject();
    newObject1.addComponent<Mesh>("Assets/Models/cube.glb");
    Transform& t2 = newObject1.addComponent<Transform>(&rotTrans);
    t2.translate(glm::vec3(-1, 0, -1));
  }

  bool open = true;
  RTIDPRR::Time::Timer timer;
  float deltaTime = 0.0f;
  do {
    timer.restart();
    open = !window.processInput(deltaTime);

    // Update test objects
    rotTrans.rotate((deltaTime * 0.00001f) * glm::vec3(0, 1, 0));
    transform.rotate((deltaTime * 0.000001f) * glm::vec3(0, 1, 0));

    // Update scene, systems and render
    scene.update(deltaTime);
    renderer.render(scene);
    timer.end();
    deltaTime = timer.getDeltaMs();
  } while (open);
  return 0;
}
