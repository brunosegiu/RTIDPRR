#include "PatchCounter.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

uint32_t PatchCounter::sWorkgroupSizeX = 8;
uint32_t PatchCounter::sWorkgroupSizeY = 8;

PatchCounter::PatchCounter() {
  CommandPool& commandPool = Context::get().getCommandPool();
  mCommand = commandPool.allocateGraphicsCommand();
}

void PatchCounter::initResources(const std::vector<const Texture*>& patchIdTexs,
                                 const uint32_t patchCount) {
  static bool init = false;
  if (!init) {
    mResources = std::make_unique<CounterResources>(patchIdTexs, patchCount);
    init = true;
  }
}

void PatchCounter::count(Scene& scene,
                         const std::vector<const Texture*>& patchIdTextures) {
  using namespace RTIDPRR::Component;
  uint32_t patchCount = Mesh::getTotalPatchCount();

  initResources(patchIdTextures, patchCount);

  {
    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
    RTIDPRR_ASSERT_VK(mCommand->begin(beginInfo));

    mCommand->beginMarker("Patch Counting");

    mCommand->beginMarker("Transition PatchId texture");
    {
      mCommand->transitionTextures(ResourceTransition::GraphicsToGraphics,
                                   vk::PipelineStageFlagBits::eBottomOfPipe,
                                   vk::PipelineStageFlagBits::eComputeShader,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   vk::AccessFlagBits::eShaderRead,
                                   patchIdTextures);
    }
    mCommand->endMarker();

    mCommand->beginMarker("Clear PatchId buffer");
    {
      mCommand->bindPipeline(*mResources->mClearPipeline);
      mCommand->bindShaderParameterGroup(0, *mResources->mClearPipeline,
                                         mResources->mPatchCountBufferParam);
      mCommand->dispatch(patchCount, 1, 1);
    }
    mCommand->endMarker();

    mCommand->beginMarker("Count Patches");
    {
      mCommand->bindPipeline(*mResources->mCountPipeline);

      mCommand->bindShaderParameterGroup(0, *mResources->mCountPipeline,
                                         mResources->mPatchCountBufferParam);
      for (size_t index = 0; index < mResources->mPatchIdImages.size();
           ++index) {
        mCommand->beginMarker("Count Patches for texture");

        mCommand->bindShaderParameterGroup(1, *(mResources->mCountPipeline),
                                           mResources->mPatchIdImages[index]);
        mCommand->dispatch(
            patchIdTextures[index]->getExtent().width / sWorkgroupSizeX,
            patchIdTextures[index]->getExtent().height / sWorkgroupSizeY, 1);
        mCommand->endMarker();
      }
    }
    mCommand->endMarker();
  }
  RTIDPRR_ASSERT_VK(mCommand->end());
  mCommand->endMarker();

  std::vector<vk::PipelineStageFlags> waitForStages{
      vk::PipelineStageFlagBits::eFragmentShader};

  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(
      *static_cast<vk::CommandBuffer*>(mCommand));
  // .setWaitSemaphores(basePassRenderer.getDoneSemaphore())
  //    .setWaitDstStageMask(waitForStages);

  Context::get().getDevice().getGraphicsQueue().submit(submitInfo);
  /*RTIDPRR_ASSERT_VK(
          Context::get().getDevice().getLogicalDeviceHandle().waitIdle());*/
  // void* map = mResources->mPatchCountBuffer.map();
  // std::vector<uint32_t> buf(static_cast<uint32_t*>(map),
  //                        static_cast<uint32_t*>(map) + patchCount);
}

PatchCounter::~PatchCounter() {}

PatchCounter::CounterResources::CounterResources(
    const std::vector<const Texture*>& patchIdTexs, const uint32_t patchCount)
    : mWorkgroupSizeConstant(vk::ShaderStageFlagBits::eCompute, sWorkgroupSizeX,
                             sWorkgroupSizeY),
      mPatchCountBuffer(patchCount * sizeof(uint32_t),
                        vk::BufferUsageFlagBits::eStorageBuffer,
                        vk::MemoryPropertyFlagBits::eDeviceLocal),
      mPatchIdImages(),
      mPatchCountBufferParam(mPatchCountBuffer, patchCount) {
  mPatchIdImages.reserve(patchIdTexs.size());
  for (const Texture* texture : patchIdTexs) {
    mPatchIdImages.emplace_back(
        vk::ShaderStageFlagBits::eCompute,
        ShaderParameterTexture(SamplerOptions{}.setTexture(texture)));
  }
  std::vector<vk::DescriptorSetLayout> layouts{
      mPatchCountBufferParam.getLayout()};
  for (const auto& imageParam : mPatchIdImages) {
    layouts.emplace_back(imageParam.getLayout());
  }
  mClearPipeline = std::make_unique<ComputePipeline>(
      "Source/Shaders/Build/CleanBuffer.comp",
      std::vector<vk::DescriptorSetLayout>{mPatchCountBufferParam.getLayout()},
      std::vector<vk::PushConstantRange>{},
      generateSpecializationMap(mWorkgroupSizeConstant));
  mCountPipeline = std::make_unique<ComputePipeline>(
      "Source/Shaders/Build/CountPatches.comp", layouts);
}

PatchCounter::CounterResources::CounterResources(
    CounterResources&& other) noexcept
    : mWorkgroupSizeConstant(std::move(other.mWorkgroupSizeConstant)),
      mPatchCountBuffer(std::move(other.mPatchCountBuffer)),
      mPatchIdImages(std::move(other.mPatchIdImages)),
      mPatchCountBufferParam(std::move(other.mPatchCountBufferParam)),
      mCountPipeline(std::move(other.mCountPipeline)) {}