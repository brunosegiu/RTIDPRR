#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Command.h"
#include "../Core/ComputePipeline.h"
#include "../Core/Shaders/ShaderParameterBuffer.h"
#include "../Core/Shaders/ShaderParameterImage.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../Geometry/IndexedVertexBuffer.h"

namespace RTIDPRR {
namespace Graphics {

class PatchCounter {
 public:
  PatchCounter(uint32_t patchCount, Texture* texture);

  void count(const std::vector<Texture*>& patchIdTex,
             vk::Semaphore& basePassDoneSemaphore);

  virtual ~PatchCounter();

 private:
  Command* mCommand;
  Buffer mBuffer;
  ShaderParameterGroup<ShaderParameterImage> mImageParam;
  ShaderParameterBuffer<uint32_t> mBufferParam;
  ComputePipeline mComputePipeline;
};

}  // namespace Graphics
}  // namespace RTIDPRR
