#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Command.h"
#include "../Core/ComputePipeline.h"
#include "../Core/Shaders/ShaderParameterBuffer.h"
#include "../Core/Shaders/ShaderParameterImage.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../DeferredRendering/BasePassRenderer.h"
#include "../Geometry/IndexedVertexBuffer.h"

namespace RTIDPRR {
namespace Graphics {

class PatchCounter {
 public:
  PatchCounter();

  void count(Scene& scene, BasePassRenderer& basePassRenderer);

  virtual ~PatchCounter();

 private:
  struct CounterResources {
    Buffer mPatchCountBuffer;
    std::vector<ShaderParameterGroup<ShaderParameterImage>> mPatchIdImages;
    ShaderParameterBuffer<uint32_t> mPatchCountBufferParam;
    std::unique_ptr<ComputePipeline> mClearPipeline;
    std::unique_ptr<ComputePipeline> mCountPipeline;

    CounterResources(const std::vector<const Texture*>& patchIdTexs,
                     const uint32_t patchCount)
        : mPatchCountBuffer(patchCount * sizeof(uint32_t),
                            vk::BufferUsageFlagBits::eStorageBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible |
                                vk::MemoryPropertyFlagBits::eHostCoherent),
          mPatchIdImages(),
          mPatchCountBufferParam(mPatchCountBuffer, patchCount) {
      mPatchIdImages.reserve(patchIdTexs.size());
      for (const Texture* texture : patchIdTexs) {
        mPatchIdImages.emplace_back(vk::ShaderStageFlagBits::eCompute,
                                    ShaderParameterImage(texture));
      }

      std::vector<vk::DescriptorSetLayout> layouts{
          mPatchCountBufferParam.getLayout()};
      for (const auto& imageParam : mPatchIdImages) {
        layouts.emplace_back(imageParam.getLayout());
      }
      mClearPipeline = std::make_unique<ComputePipeline>(
          "Source/Shaders/Build/CleanBuffer.comp",
          std::vector<vk::DescriptorSetLayout>{
              mPatchCountBufferParam.getLayout()});
      mCountPipeline = std::make_unique<ComputePipeline>(
          "Source/Shaders/Build/CountPatches.comp", layouts);
    }

    CounterResources(CounterResources&& other) noexcept
        : mPatchCountBuffer(std::move(other.mPatchCountBuffer)),
          mPatchIdImages(std::move(other.mPatchIdImages)),
          mPatchCountBufferParam(std::move(other.mPatchCountBufferParam)),
          mCountPipeline(std::move(other.mCountPipeline)) {}
  };

  Command* mCommand;
  std::unique_ptr<CounterResources> mResources;

  void initResources(const std::vector<const Texture*>& patchIdTexs,
                     const uint32_t patchCount);
};

}  // namespace Graphics
}  // namespace RTIDPRR
