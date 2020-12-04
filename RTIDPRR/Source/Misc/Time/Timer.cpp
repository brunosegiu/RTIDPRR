#include "Timer.h"

using namespace RTIDPRR::Time;

Timer::Timer() : mHasRegisteredCycle(false) {
  mStartTime = std::chrono::system_clock::now();
}

void Timer::end() {
  mEndTime = std::chrono::system_clock::now();
  mHasRegisteredCycle = true;
}

void Timer::restart() {
  mStartTime = std::chrono::system_clock::now();
  mHasRegisteredCycle = false;
}

float Timer::getDeltaMs() const {
  float duration = mHasRegisteredCycle
                       ? std::chrono::duration_cast<std::chrono::microseconds>(
                             mEndTime - mStartTime)
                             .count()
                       : 0.0f;
  return duration < 0.0f ? 0.0f : duration;
}

Timer::~Timer() {}
