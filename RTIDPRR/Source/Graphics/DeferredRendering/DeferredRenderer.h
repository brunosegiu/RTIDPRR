#pragma once

#include <vulkan/vulkan.hpp>

#include "../Core/Context.h"
#include "../Core/Pipeline.h"

namespace RTIDPRR {
namespace Graphics {
class DeferredRenderer {
 public:
  DeferredRenderer();

  void render();

  virtual ~DeferredRenderer();

 private:
  Pipeline mPipeline;

};
}  // namespace Graphics
}  // namespace RTIDPRR
