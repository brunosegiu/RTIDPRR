#pragma once

#include <vulkan/vulkan.hpp>

#include "../../Misc/Window.h"

namespace RTIDPRR {
namespace Graphics {
class Instance {
 public:
  Instance(const Window& window);

  const vk::SurfaceKHR initSurface(const Window& window) const;

  const vk::Instance& getHandle() const { return mInstanceHandle; };
  const vk::DispatchLoaderDynamic& getDynamicDispatcher() const {
    return mDynamicDispatcher;
  }
  bool getAreMarkersSupported() const { return mAreMarkersSupported; }

  virtual ~Instance();

 private:
  Instance(Instance const&) = delete;
  Instance& operator=(Instance const&) = delete;

  vk::Instance mInstanceHandle;
  vk::DispatchLoaderDynamic mDynamicDispatcher;
  bool mAreMarkersSupported;

#if defined(_DEBUG)
  vk::DebugUtilsMessengerEXT mDebugMessenger;
#endif
};
}  // namespace Graphics
}  // namespace RTIDPRR
