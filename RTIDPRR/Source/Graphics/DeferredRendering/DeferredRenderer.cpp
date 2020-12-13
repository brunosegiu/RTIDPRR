#include "DeferredRenderer.h"

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

static const vk::Extent2D DEFERRED_RESOLUTION = vk::Extent2D(1920, 1080);

DeferredRenderer::DeferredRenderer()
    : mLightPassResources(Context::get().getSwapchain().getExtent(),
                          mBasePassResources.mAlbedoTex,
                          mBasePassResources.mNormalTex,
                          mBasePassResources.mDepthTex),
      mBasePassResources(DEFERRED_RESOLUTION) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  vk::CommandBufferAllocateInfo commandAllocInfo =
      vk::CommandBufferAllocateInfo()
          .setCommandPool(graphicsQueue.getCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1);

  std::vector<vk::CommandBuffer> commandBuffers = RTIDPRR_ASSERT_VK(
      device.getLogicalDeviceHandle().allocateCommandBuffers(commandAllocInfo));

  mCommandBuffer = commandBuffers[0];
}

void DeferredRenderer::render(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();

  swapchain.swapBuffers();

  renderBasePass(scene);

  // TODO: REMOVE
  RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().waitIdle());

  {
    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
    RTIDPRR_ASSERT_VK(mCommandBuffer.begin(beginInfo));
    vk::ImageMemoryBarrier imageBarrier =
        vk::ImageMemoryBarrier()
            .setOldLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImage(mBasePassResources.mDepthTex.getImage())
            .setSubresourceRange(
                vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setLayerCount(1));
    mCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
                                   vk::PipelineStageFlagBits::eTopOfPipe, {},
                                   nullptr, nullptr, imageBarrier);
    RTIDPRR_ASSERT_VK(mCommandBuffer.end());
    vk::SubmitInfo submitInfo =
        vk::SubmitInfo().setCommandBuffers(mCommandBuffer);
    graphicsQueue.submit(submitInfo);
  }
  // TODO: REMOVE
  RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().waitIdle());

  renderLightPass(scene);

  swapchain.submitCommand(mCommandBuffer);

  // TODO: REMOVE
  RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().waitIdle());
}

void DeferredRenderer::renderBasePass(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();

  const vk::ClearColorValue albedoClearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
  const vk::ClearColorValue normalClearColor(
      std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
  const vk::ClearDepthStencilValue depthClearColor =
      vk::ClearDepthStencilValue(1.0f, 1u);
  const std::vector<vk::ClearValue> clearValues{
      albedoClearColor, normalClearColor, depthClearColor};

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mCommandBuffer.begin(beginInfo));

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(mBasePassResources.mBasePass.getHandle())
          .setFramebuffer(mBasePassResources.mGBuffer.getHandle())
          .setRenderArea({vk::Offset2D{0, 0}, DEFERRED_RESOLUTION})
          .setClearValues(clearValues);
  mCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                 vk::SubpassContents::eInline);
  mCommandBuffer.bindPipeline(
      vk::PipelineBindPoint::eGraphics,
      mBasePassResources.mBasePassPipeline.getPipelineHandle());

  const vk::Viewport viewport{
      0.0f,
      0.0f,
      static_cast<float>(mBasePassResources.mGBuffer.getWidth()),
      static_cast<float>(mBasePassResources.mGBuffer.getHeight()),
      0.0f,
      1.0f};
  mCommandBuffer.setViewport(0, viewport);

  {
    using namespace RTIDPRR::Core;
    using namespace RTIDPRR::Component;
    System<Mesh>& meshSystem = scene.getSystem<System<Mesh>>();

    glm::mat4 viewProjection = scene.getCamera().getViewProjection();

    for (Mesh& mesh : meshSystem.getComponents()) {
      if (Object* obj = mesh.getObject()) {
        Transform* transform = obj->getComponent<Transform>();
        if (transform) {
          const glm::mat4 modelMatrix = transform->getAbsoluteTransform();
          glm::mat4 modelViewProjection = viewProjection * modelMatrix;

          std::vector<CameraMatrices> matrices{
              {modelMatrix, modelViewProjection}};

          mCommandBuffer.pushConstants<CameraMatrices>(
              mBasePassResources.mBasePassPipeline.getPipelineLayout(),
              vk::ShaderStageFlagBits::eVertex, 0, matrices);
          mesh.draw(mCommandBuffer);
        }
      }
    }
  }
  mCommandBuffer.endRenderPass();
  RTIDPRR_ASSERT_VK(mCommandBuffer.end());
  vk::SubmitInfo submitInfo =
      vk::SubmitInfo().setCommandBuffers(mCommandBuffer);
  graphicsQueue.submit(submitInfo);
}

void DeferredRenderer::renderLightPass(Scene& scene) {
  Swapchain& swapchain = Context::get().getSwapchain();

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mCommandBuffer.begin(beginInfo));

  const vk::ClearColorValue clearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});

  const std::vector<vk::ClearValue> clearValues{clearColor};

  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Component;

  const std::vector<Light>& lights =
      scene.getSystem<System<Light>>().getComponents();
  for (uint32_t index = 0; index < lights.size(); ++index) {
    const Light& light = lights[index];
    std::get<3>(
        mLightPassResources.mFragmentStageParameters.getShaderParameters())
        .update(index, light.getProxy());
  }

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass().getHandle())
          .setFramebuffer(swapchain.getCurrentFramebuffer().getHandle())
          .setRenderArea(
              {vk::Offset2D{0, 0},
               vk::Extent2D{swapchain.getCurrentFramebuffer().getWidth(),
                            swapchain.getCurrentFramebuffer().getHeight()}})
          .setClearValues(clearValues);

  mCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                 vk::SubpassContents::eInline);

  mCommandBuffer.bindPipeline(
      vk::PipelineBindPoint::eGraphics,
      mLightPassResources.mLightPassPipeline.getPipelineHandle());

  const vk::Viewport viewport{
      0.0f,
      0.0f,
      static_cast<float>(swapchain.getCurrentFramebuffer().getWidth()),
      static_cast<float>(swapchain.getCurrentFramebuffer().getHeight()),
      0.0f,
      1.0f};
  mCommandBuffer.setViewport(0, viewport);

  mCommandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      mLightPassResources.mLightPassPipeline.getPipelineLayout(), 0,
      mLightPassResources.mFragmentStageParameters.getDescriptorSet(), nullptr);

  mCommandBuffer.draw(3, 1, 0, 0);

  mCommandBuffer.endRenderPass();
  RTIDPRR_ASSERT_VK(mCommandBuffer.end());
}

DeferredRenderer::~DeferredRenderer() {
  const Device& device = Context::get().getDevice();
  device.getLogicalDeviceHandle().freeCommandBuffers(
      device.getGraphicsQueue().getCommandPool(), mCommandBuffer);
}

static const vk::Format albedoFormat = vk::Format::eR8G8B8A8Unorm;
static const vk::Format normalFormat = vk::Format::eR16G16B16A16Sfloat;
static const vk::Format depthFormat = vk::Format::eD32Sfloat;

BasePassResources::BasePassResources(const vk::Extent2D& extent)
    : mAlbedoTex(extent, albedoFormat,
                 vk::ImageUsageFlagBits::eColorAttachment |
                     vk::ImageUsageFlagBits::eSampled,
                 vk::ImageAspectFlagBits::eColor),
      mNormalTex(extent, normalFormat,
                 vk::ImageUsageFlagBits::eColorAttachment |
                     vk::ImageUsageFlagBits::eSampled,
                 vk::ImageAspectFlagBits::eColor),
      mDepthTex(extent, depthFormat,
                vk::ImageUsageFlagBits::eDepthStencilAttachment |
                    vk::ImageUsageFlagBits::eSampled,
                vk::ImageAspectFlagBits::eDepth),
      mBasePass({&mAlbedoTex, &mNormalTex, &mDepthTex}, true),
      mGBuffer(Context::get().getDevice(), mBasePass.getHandle(),
               {mAlbedoTex.getImageView(), mNormalTex.getImageView(),
                mDepthTex.getImageView()},
               extent.width, extent.height),
      mInlineParameters({vk::ShaderStageFlagBits::eVertex}),
      mBasePassPipeline(mBasePass, extent, {},
                        mInlineParameters.getPushConstantRanges()) {}

RTIDPRR::Graphics::LightPassResources::LightPassResources(
    const vk::Extent2D& extent, const Texture& albedoTex,
    const Texture& normalTex, const Texture& depthTex)
    : mFragmentStageParameters(
          vk::ShaderStageFlagBits::eFragment, ShaderParameterTexture(albedoTex),
          ShaderParameterTexture(normalTex), ShaderParameterTexture(depthTex),
          ShaderParameterArray<RTIDPRR::Component::LightProxy>(3)),
      mLightPassPipeline(Context::get().getSwapchain().getMainRenderPass(),
                         extent,
                         std::vector<vk::DescriptorSetLayout>{
                             mFragmentStageParameters.getLayout()},
                         {}) {}
