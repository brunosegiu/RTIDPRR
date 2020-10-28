#include "Context.h"

using namespace RTIDPRR::Graphics;

Context::Context(const SDL_Window* window) : mInstance(window) {
	mWindowSurface = mInstance.initSurface(window);

}

Context::~Context() {}
