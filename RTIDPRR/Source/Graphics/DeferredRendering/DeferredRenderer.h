#pragma once

#include <vulkan/vulkan.hpp>

#include "../Core/Context.h"

namespace RTIDPRR {
namespace Graphics {
class DeferredRenderer {
 public:
  DeferredRenderer();

  void render();

  virtual ~DeferredRenderer();

 private:
};
}  // namespace Graphics
}  // namespace RTIDPRR
