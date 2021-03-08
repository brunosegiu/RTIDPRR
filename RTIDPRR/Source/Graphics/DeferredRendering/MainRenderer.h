#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../CountPatches/PatchCounter.h"
#include "../ShadowRendering/ShadowRenderer.h"
#include "BasePassRenderer.h"
#include "LightingPassRenderer.h"

namespace RTIDPRR {
namespace Graphics {

class MainRenderer {
 public:
  MainRenderer();

  void render(Scene& scene);

  virtual ~MainRenderer();

 private:
  ShadowRenderer mShadowRenderer;
  BasePassRenderer mBasePassRenderer;
  LightingPassRenderer mLightingPassRenderer;
  PatchCounter mPatchCounter;
};

}  // namespace Graphics
}  // namespace RTIDPRR
