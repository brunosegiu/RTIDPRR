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

  virtual ~Instance();

 private:
  Instance(Instance const&) = delete;
  Instance& operator=(Instance const&) = delete;

  static const std::vector<const char*> getRequiredExtensions(
      const Window& window);

  vk::Instance mInstanceHandle;

#if defined(_DEBUG)
  vk::DebugUtilsMessengerEXT mDebugMessenger;
#endif
};
}  // namespace Graphics
}  // namespace RTIDPRR
