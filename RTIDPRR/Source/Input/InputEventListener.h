#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>

namespace RTIDPRR {
namespace Input {
class InputEventListener {
 public:
  InputEventListener(){};

  virtual void onKeyPress(const float timeDelta, const SDL_Scancode key){};

  virtual void onCursorMovement(const float timeDelta,
                                const glm::vec2& position){};
  virtual void onRightClick(const float timeDelta){};
  virtual void onLeftClick(const float timeDelta){};
  virtual void onRightClickReleased(const float timeDelta){};
  virtual void onLeftClickReleased(const float timeDelta){};

  virtual void onQuit(){};

  virtual void onInputProcessed(){};

  virtual ~InputEventListener(){};
};
}  // namespace Input
}  // namespace RTIDPRR
