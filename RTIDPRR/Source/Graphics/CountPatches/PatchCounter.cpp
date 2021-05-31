#include "PatchCounter.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

uint32_t PatchCounter::sDispatchSizeX = 8;
uint32_t PatchCounter::sDispatchSizeY = 8;

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

void PatchCounter::count(Scene& scene, BasePassRenderer& basePassRenderer) {
  using namespace RTIDPRR::Component;
  uint32_t patchCount = Mesh::getTotalPatchCount();

  std::vector<const Texture*> patchIdTextures{
      &basePassRenderer.getResources().mPatchIdTex};

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
        mCommand->dispatch(patchIdTextures[index]->getExtent().width / sDispatchSizeX,
                           patchIdTextures[index]->getExtent().height / sDispatchSizeY, 1);
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
