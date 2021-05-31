#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Command.h"
#include "../Core/ComputePipeline.h"
#include "../Core/Shaders/ShaderParameterBuffer.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../DeferredRendering/BasePassRenderer.h"
#include "../Geometry/IndexedVertexBuffer.h"

namespace RTIDPRR {
namespace Graphics {

class PatchCounter {
 public:
  PatchCounter();

  // These two are hardcoded in the shader as a workaround
  static uint32_t sWorkgroupSizeX, sWorkgroupSizeY;

  void count(Scene& scene, const std::vector<const Texture*>& patchIdTextures);

  virtual ~PatchCounter();

 private:
  struct CounterResources {
    ShaderParameterConstantGroup<uint32_t, uint32_t> mWorkgroupSizeConstant;
    Buffer mPatchCountBuffer;
    std::vector<ShaderParameterGroup<ShaderParameterTexture>> mPatchIdImages;
    ShaderParameterBuffer<uint32_t> mPatchCountBufferParam;
    std::unique_ptr<ComputePipeline> mClearPipeline;
    std::unique_ptr<ComputePipeline> mCountPipeline;

    CounterResources(const std::vector<const Texture*>& patchIdTexs,
                     const uint32_t patchCount);

    CounterResources(CounterResources&& other) noexcept;
  };

  Command* mCommand;
  std::unique_ptr<CounterResources> mResources;

  void initResources(const std::vector<const Texture*>& patchIdTexs,
                     const uint32_t patchCount);
};

}  // namespace Graphics
}  // namespace RTIDPRR
