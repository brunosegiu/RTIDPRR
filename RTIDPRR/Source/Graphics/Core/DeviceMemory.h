#pragma once

#include <vulkan/vulkan.hpp>

#include "../../Misc/DebugUtils.h"
#include "Context.h"

namespace RTIDPRR {
namespace Graphics {
namespace DeviceMemory {

static uint32_t findMemoryIndex(const uint32_t filter,
                                const vk::MemoryPropertyFlags properties) {
  const vk::PhysicalDevice& physicalDevice =
      RTIDPRR::Graphics::Context::get().getDevice().getPhysicalDevice();
  vk::PhysicalDeviceMemoryProperties physicalMemoryProperties =
      physicalDevice.getMemoryProperties();
  for (uint32_t memoryIndex = 0;
       memoryIndex < physicalMemoryProperties.memoryTypeCount; ++memoryIndex) {
    if ((filter & (1 << memoryIndex)) &&
        (physicalMemoryProperties.memoryTypes[memoryIndex].propertyFlags &
         properties) == properties) {
      return memoryIndex;
    }
  }
  RTIDPRR_ASSERT_MSG(false, "Failed to find suitable memory index.");
  return 0;
}

}  // namespace DeviceMemory
}  // namespace Graphics
}  // namespace RTIDPRR
