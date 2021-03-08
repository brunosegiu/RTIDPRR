#include "PatchCounter.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

PatchCounter::PatchCounter() {
  CommandPool& commandPool = Context::get().getCommandPool();
  mCommand = commandPool.allocateComputeCommand();
}

void PatchCounter::initResources(const std::vector<const Texture*>& patchIdTexs,
                                 const uint32_t patchCount) {
  mResources = std::make_unique<CounterResources>(patchIdTexs, patchCount);
}

void PatchCounter::count(Scene& scene, BasePassRenderer& basePassRenderer) {
  using namespace RTIDPRR::Component;
  uint32_t patchCount = Mesh::getTotalPatchCount();

  std::vector<const Texture*> patchIdTextures{
      &basePassRenderer.getResources().mPatchIdTex};

  initResources(patchIdTextures, patchCount);

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mCommand->begin(beginInfo));

  mCommand->transitionTextures(
      ResourceTransition::GraphicsToCompute,
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eComputeShader, vk::ImageLayout::eUndefined,
      vk::ImageLayout::eGeneral, vk::AccessFlagBits::eShaderRead,
      patchIdTextures);

  mCommand->bindPipeline(*mResources->mClearPipeline);
  mCommand->bindShaderParameterGroup(0, *mResources->mClearPipeline,
                                     mResources->mPatchCountBufferParam);
  mCommand->dispatch(patchCount, 1, 1);

  mCommand->bindPipeline(*mResources->mCountPipeline);

  mCommand->bindShaderParameterGroup(0, *mResources->mCountPipeline,
                                     mResources->mPatchCountBufferParam);
  for (size_t index = 0; index < mResources->mPatchIdImages.size(); ++index) {
    mCommand->bindShaderParameterGroup(1, *(mResources->mCountPipeline),
                                       mResources->mPatchIdImages[index]);

    mCommand->dispatch(patchIdTextures[index]->getExtent().width,
                       patchIdTextures[index]->getExtent().height, 1);
  }

  RTIDPRR_ASSERT_VK(mCommand->end());

  std::vector<vk::PipelineStageFlags> waitForStages{
      vk::PipelineStageFlagBits::eFragmentShader};

  vk::SubmitInfo submitInfo =
      vk::SubmitInfo()
          .setCommandBuffers(*static_cast<vk::CommandBuffer*>(mCommand))
          .setWaitSemaphores(basePassRenderer.getDoneSemaphore())
          .setWaitDstStageMask(waitForStages);

  Context::get().getDevice().getComputeQueue().submit(submitInfo);
  RTIDPRR_ASSERT_VK(
      Context::get().getDevice().getLogicalDeviceHandle().waitIdle());
  void* map = mResources->mPatchCountBuffer.map();
  std::vector<uint32_t> buf(static_cast<uint32_t*>(map),
                            static_cast<uint32_t*>(map) + patchCount);
}

PatchCounter::~PatchCounter() {}
