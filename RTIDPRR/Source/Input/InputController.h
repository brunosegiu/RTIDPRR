#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <vector>

#include "InputEventListener.h"

namespace RTIDPRR {
class Window;
}

namespace RTIDPRR {
namespace Input {
class InputController {
 public:
  InputController(RTIDPRR::Window* window);

  void attach(InputEventListener* listener);
  bool processEvents(const float timeDelta);

  virtual ~InputController();

 private:
  std::vector<InputEventListener*> mListeners;

  RTIDPRR::Window* mWindow;
};

}  // namespace Input
}  // namespace RTIDPRR
