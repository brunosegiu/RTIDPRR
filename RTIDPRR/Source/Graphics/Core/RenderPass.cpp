#include "RenderPass.h"

#include "Context.h"

using namespace RTIDPRR::Graphics;

RenderPass::RenderPass(const std::vector<const Texture*>& renderTargets,
                       bool depthTestEnabled)
    : mDepthTestEnabled(depthTestEnabled),
      mColorAttachmentCount(depthTestEnabled ? renderTargets.size() - 1
                                             : renderTargets.size()),
      mIsHandleOwner(true) {
  const Device& device = Context::get().getDevice();

  std::vector<vk::AttachmentDescription> attachments;
  std::vector<vk::AttachmentReference> colorReferences;
  attachments.reserve(renderTargets.size());
  colorReferences.reserve(renderTargets.size());
  for (uint32_t index = 0; index < mColorAttachmentCount; ++index) {
    const Texture& renderTarget = *renderTargets[index];
    attachments.emplace_back(
        vk::AttachmentDescription()
            .setFormat(renderTarget.getFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal));
    colorReferences.emplace_back(
        vk::AttachmentReference()
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(index));
  }

  const Texture& lastRenderTarget = *renderTargets.back();

  vk::AttachmentDescription depthAttachmentDescription =
      vk::AttachmentDescription()
          .setFormat(lastRenderTarget.getFormat())
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  if (depthTestEnabled) {
    attachments.emplace_back(depthAttachmentDescription);
  }

  vk::AttachmentReference depthReference =
      vk::AttachmentReference()
          .setAttachment(mColorAttachmentCount)
          .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  vk::SubpassDescription subpass =
      vk::SubpassDescription()
          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
          .setColorAttachments(colorReferences)
          .setPDepthStencilAttachment(depthTestEnabled ? &depthReference
                                                       : nullptr);

  vk::RenderPassCreateInfo renderPassCreateInfo =
      vk::RenderPassCreateInfo()
          .setAttachments(attachments)
          .setSubpasses(subpass);

  mHandle =
      device.getLogicalDeviceHandle().createRenderPass(renderPassCreateInfo);
}

RenderPass::RenderPass(vk::RenderPass renderPass, uint32_t colorAttachmentCount,
                       bool depthTestEnabled)
    : mHandle(renderPass),
      mColorAttachmentCount(colorAttachmentCount),
      mDepthTestEnabled(depthTestEnabled),
      mIsHandleOwner(false) {}

RenderPass::~RenderPass() {
  if (mIsHandleOwner) {
    const Device& device = Context::get().getDevice();
    device.getLogicalDeviceHandle().destroyRenderPass(mHandle);
  }
}
