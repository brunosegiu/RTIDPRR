#include "InputController.h"

#include "../Misc/Window.h"

using namespace RTIDPRR::Input;

InputController::InputController(RTIDPRR::Window* window) : mWindow(window) {}

void InputController::attach(InputEventListener* listener) {
  mListeners.push_back(listener);
}

bool InputController::processEvents(const float timeDelta) {
  glm::ivec2 mouseState;
  SDL_GetMouseState(&mouseState.x, &mouseState.y);
  const glm::vec2 mousePosNorm(
      (0.5f - static_cast<float>(mouseState.x) / mWindow->getWidth()),
      (0.5f - static_cast<float>(mouseState.y) / mWindow->getHeight()));
  for (InputEventListener* listener : mListeners) {
    listener->onCursorMovement(timeDelta, mousePosNorm);
  }

  SDL_Event evt;
  while (SDL_PollEvent(&evt)) {
    const bool shouldQuit =
        (evt.type == SDL_QUIT ||
         (evt.type == SDL_KEYDOWN && (evt.key.keysym.sym == SDLK_ESCAPE)));
    if (shouldQuit) {
      return true;
      for (const auto& listener : mListeners) {
        listener->onQuit();
      }
    }

    if (evt.type == SDL_MOUSEBUTTONDOWN) {
      if (evt.button.button == SDL_BUTTON_LEFT) {
        for (const auto& listener : mListeners) {
          listener->onLeftClick(timeDelta);
        }
      }
      if (evt.button.button == SDL_BUTTON_RIGHT) {
        for (const auto& listener : mListeners) {
          listener->onRightClick(timeDelta);
        }
      }
    }
    if (evt.type == SDL_MOUSEBUTTONUP) {
      if (evt.button.button == SDL_BUTTON_LEFT) {
        for (const auto& listener : mListeners) {
          listener->onLeftClickReleased(timeDelta);
        }
      }
      if (evt.button.button == SDL_BUTTON_RIGHT) {
        for (const auto& listener : mListeners) {
          listener->onRightClickReleased(timeDelta);
        }
      }
    }
  }

  int keyCount = 0;
  const Uint8* pressedKeys = SDL_GetKeyboardState(&keyCount);
  for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex) {
    if (pressedKeys[keyIndex]) {
      for (const auto& listener : mListeners) {
        listener->onKeyPress(timeDelta, static_cast<SDL_Scancode>(keyIndex));
      }
    }
  }
  for (const auto& listener : mListeners) {
    listener->onInputProcessed();
  }
  return false;
}

InputController::~InputController() {}
