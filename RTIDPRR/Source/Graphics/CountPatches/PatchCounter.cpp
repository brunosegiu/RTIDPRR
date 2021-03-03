#include "PatchCounter.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

PatchCounter::PatchCounter(uint32_t patchCount, Texture* texture)
    : mBuffer(patchCount * sizeof(uint32_t),
              vk::BufferUsageFlagBits::eStorageBuffer,
              vk::MemoryPropertyFlagBits::eHostVisible |
                  vk::MemoryPropertyFlagBits::eHostCoherent),
      mImageParam(vk::ShaderStageFlagBits::eCompute,
                  ShaderParameterImage(texture)),
      mBufferParam(mBuffer, patchCount),
      mComputePipeline("Source/Shaders/Build/CountPatches.comp",
                       {mBufferParam.getLayout(), mImageParam.getLayout()}) {
  CommandPool& commandPool = Context::get().getCommandPool();
  mCommand = commandPool.allocateComputeCommand();
}

void PatchCounter::count(const std::vector<Texture*>& patchIdTex,
                         vk::Semaphore& basePassDoneSemaphore) {
  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mCommand->begin(beginInfo));

  {
    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eGeneral)
        .setImage(patchIdTex[0]->getImage())
        .setSrcQueueFamilyIndex(
            Context::get().getDevice().getGraphicsQueue().getFamilyIndex())
        .setDstQueueFamilyIndex(
            Context::get().getDevice().getComputeQueue().getFamilyIndex())
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1));
    mCommand->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                              vk::PipelineStageFlagBits::eComputeShader, {},
                              nullptr, nullptr, barrier);
  }

  mCommand->bindPipeline(mComputePipeline);

  mCommand->bindShaderParameterGroup(0, mComputePipeline, mBufferParam);
  mCommand->bindShaderParameterGroup(1, mComputePipeline, mImageParam);

  mCommand->dispatch(patchIdTex[0]->getExtent().width,
                     patchIdTex[0]->getExtent().height, 1);

  {
    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(vk::ImageLayout::eGeneral)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImage(patchIdTex[0]->getImage())
        .setSrcQueueFamilyIndex(
            Context::get().getDevice().getComputeQueue().getFamilyIndex())
        .setDstQueueFamilyIndex(
            Context::get().getDevice().getGraphicsQueue().getFamilyIndex())
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1));
    mCommand->pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader,
                              vk::PipelineStageFlagBits::eTopOfPipe, {},
                              nullptr, nullptr, barrier);
  }

  RTIDPRR_ASSERT_VK(mCommand->end());

  std::vector<vk::PipelineStageFlags> waitForStages{
      vk::PipelineStageFlagBits::eFragmentShader};

  vk::SubmitInfo submitInfo =
      vk::SubmitInfo()
          .setCommandBuffers(*static_cast<vk::CommandBuffer*>(mCommand))
          .setWaitSemaphores(basePassDoneSemaphore)
          .setWaitDstStageMask(waitForStages);
  Context::get().getDevice().getComputeQueue().submit(submitInfo);
  Context::get().getDevice().getLogicalDeviceHandle().waitIdle();
  void* map = mBuffer.map();
  std::vector<uint32_t> buf(static_cast<uint32_t*>(map),
                            static_cast<uint32_t*>(map) + 100);
}

PatchCounter::~PatchCounter() {}
