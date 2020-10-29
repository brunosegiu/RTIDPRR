#include "Queue.h"

using namespace RTIDPRR::Graphics;

Queue::Queue(const uint32_t queueFamilyIndex, const uint32_t queueIndex, const vk::Device& device) : mFamilyIndex(queueFamilyIndex), mIndex(queueIndex) {
	mHandle = device.getQueue(queueFamilyIndex, queueIndex);
}

Queue::~Queue() {}
