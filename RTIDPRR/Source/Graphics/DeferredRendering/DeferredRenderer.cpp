#include "DeferredRenderer.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

static const vk::Extent2D DEFERRED_RESOLUTION = vk::Extent2D(1920, 1080);

DeferredRenderer::DeferredRenderer()
    : mShadowRenderer(),
      mBasePassResources(DEFERRED_RESOLUTION),
      mLightPassResources(
          Context::get().getSwapchain().getExtent(),
          mBasePassResources.mAlbedoTex, mBasePassResources.mNormalTex,
          mBasePassResources.mPositionTex, mBasePassResources.mDepthTex,
          mShadowRenderer.getSamplerOptionsFromResources()),
      mPatchCounter(100, &mBasePassResources.mPatchIdTex) {
  const Device& device = Context::get().getDevice();

  CommandPool& commandPool = Context::get().getCommandPool();

  mTransitionDepthSceneCommand = commandPool.allocateGraphicsCommand();
  mBasePassCommand = commandPool.allocateGraphicsCommand();
  mLightPassCommand = commandPool.allocateGraphicsCommand();

  mCounterWaitSemaphore =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createSemaphore(
          vk::SemaphoreCreateInfo()));
}

void DeferredRenderer::render(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();

  swapchain.swapBuffers();

  mShadowRenderer.render(scene);

  renderBasePass(scene);

  mPatchCounter.count({&mBasePassResources.mPatchIdTex}, mCounterWaitSemaphore);

  renderLightPass(scene);

  swapchain.present(*mLightPassCommand);
}

void DeferredRenderer::renderBasePass(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();

  const vk::ClearColorValue albedoClearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
  const vk::ClearColorValue normalClearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
  const vk::ClearColorValue positionClearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
  const vk::ClearColorValue patchIdClearColor(
      std::array<uint32_t, 4>{0, 0, 0, 0});
  const vk::ClearDepthStencilValue depthClearColor =
      vk::ClearDepthStencilValue(1.0f, 1u);
  const std::vector<vk::ClearValue> clearValues{
      albedoClearColor, normalClearColor, positionClearColor, patchIdClearColor,
      depthClearColor};

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mBasePassCommand->begin(beginInfo));

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(mBasePassResources.mBasePass.getHandle())
          .setFramebuffer(mBasePassResources.mGBuffer.getHandle())
          .setRenderArea({vk::Offset2D{0, 0}, DEFERRED_RESOLUTION})
          .setClearValues(clearValues);
  mBasePassCommand->beginRenderPass(renderPassBeginInfo,
                                    vk::SubpassContents::eInline);
  mBasePassCommand->bindPipeline(mBasePassResources.mBasePassPipeline);

  const vk::Viewport viewport{
      0.0f,
      0.0f,
      static_cast<float>(mBasePassResources.mGBuffer.getWidth()),
      static_cast<float>(mBasePassResources.mGBuffer.getHeight()),
      0.0f,
      1.0f};
  mBasePassCommand->setViewport(0, viewport);
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

          std::vector<BasePassPushParams> matrices{
              {modelMatrix, modelViewProjection, mesh.getStartingIndex()}};

          const Frustum& cameraFrustum = scene.getCamera().getFrustum();

          if (cameraFrustum.intersects(modelMatrix, mesh.getAABB())) {
            mBasePassCommand->pushConstants<BasePassPushParams>(
                mBasePassResources.mBasePassPipeline.getPipelineLayout(),
                vk::ShaderStageFlagBits::eVertex, 0, matrices);
            mBasePassCommand->bindMesh(mesh.getIndexedBuffer());
            mBasePassCommand->drawMesh(mesh.getIndexedBuffer());
          }
        }
      }
    }
  }

  mBasePassCommand->endRenderPass();

  {
    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eGeneral)
        .setImage(mBasePassResources.mPatchIdTex.getImage())
        .setSrcQueueFamilyIndex(
            Context::get().getDevice().getGraphicsQueue().getFamilyIndex())
        .setDstQueueFamilyIndex(
            Context::get().getDevice().getComputeQueue().getFamilyIndex())
        .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1));
    mBasePassCommand->pipelineBarrier(
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eBottomOfPipe, {}, nullptr, nullptr,
        barrier);
  }

  RTIDPRR_ASSERT_VK(mBasePassCommand->end());
  vk::SubmitInfo submitInfo =
      vk::SubmitInfo()
          .setCommandBuffers(*static_cast<vk::CommandBuffer*>(mBasePassCommand))
          .setSignalSemaphores(mCounterWaitSemaphore);
  graphicsQueue.submit(submitInfo);
}

void DeferredRenderer::renderLightPass(Scene& scene) {
  Swapchain& swapchain = Context::get().getSwapchain();

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mLightPassCommand->begin(beginInfo));

  mLightPassCommand->pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
                                     vk::PipelineStageFlagBits::eTopOfPipe, {},
                                     nullptr, nullptr, nullptr);

  // Transition scene depth texture
  {
    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImage(mBasePassResources.mDepthTex.getImage())
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1));
    mLightPassCommand->pipelineBarrier(
        vk::PipelineStageFlagBits::eLateFragmentTests,
        vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr,
        barrier);
  }

  mShadowRenderer.transitionShadowTexturesReadOnly(*mLightPassCommand);

  const vk::ClearColorValue clearColor(
      std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});

  const std::vector<vk::ClearValue> clearValues{clearColor};

  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Component;

  const std::vector<Light>& lights =
      scene.getSystem<LightSystem>().getComponents();
  for (uint32_t index = 0;
       index < std::min(static_cast<uint32_t>(lights.size()),
                        ShadowRenderer::sMaxLightsToRender);
       ++index) {
    const Light& light = lights[index];
    std::get<4>(
        mLightPassResources.mFragmentStageParameters.getShaderParameters())
        .update(index, light.getProxy());
  }

  const Framebuffer& currentFramebuffer = swapchain.getCurrentFramebuffer();

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass().getHandle())
          .setFramebuffer(currentFramebuffer.getHandle())
          .setRenderArea({vk::Offset2D(0, 0),
                          vk::Extent2D(currentFramebuffer.getWidth(),
                                       currentFramebuffer.getHeight())})
          .setClearValues(clearValues);

  mLightPassCommand->beginRenderPass(renderPassBeginInfo,
                                     vk::SubpassContents::eInline);

  mLightPassCommand->bindPipeline(mLightPassResources.mLightPassPipeline);

  const vk::Viewport viewport(
      0.0f, 0.0f, static_cast<float>(currentFramebuffer.getWidth()),
      static_cast<float>(currentFramebuffer.getHeight()), 0.0f, 1.0f);
  mLightPassCommand->setViewport(0, viewport);

  mLightPassCommand->bindShaderParameterGroup(
      0, mLightPassResources.mLightPassPipeline,
      mLightPassResources.mFragmentStageParameters);

  std::vector<CameraMatrices> matrices{
      {glm::inverse(scene.getCamera().getProjection()),
       glm::inverse(scene.getCamera().getView())}};

  mLightPassCommand->pushConstants<CameraMatrices>(
      mLightPassResources.mLightPassPipeline.getPipelineLayout(),
      vk::ShaderStageFlagBits::eFragment, 0, matrices);

  mLightPassCommand->draw(3, 1, 0, 0);
  mLightPassCommand->endRenderPass();
  RTIDPRR_ASSERT_VK(mLightPassCommand->end());
}

DeferredRenderer::~DeferredRenderer() {}

static const vk::Format albedoFormat = vk::Format::eR8G8B8A8Unorm;
static const vk::Format normalFormat = vk::Format::eR16G16B16A16Sfloat;
static const vk::Format positionFormat = vk::Format::eR16G16B16A16Sfloat;
static const vk::Format patchIdFormat = vk::Format::eR32Uint;
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
      mPositionTex(extent, positionFormat,
                   vk::ImageUsageFlagBits::eColorAttachment |
                       vk::ImageUsageFlagBits::eSampled,
                   vk::ImageAspectFlagBits::eColor),
      mPatchIdTex(extent, patchIdFormat,
                  vk::ImageUsageFlagBits::eColorAttachment |
                      vk::ImageUsageFlagBits::eSampled |
                      vk::ImageUsageFlagBits::eStorage,
                  vk::ImageAspectFlagBits::eColor),
      mDepthTex(extent, depthFormat,
                vk::ImageUsageFlagBits::eDepthStencilAttachment |
                    vk::ImageUsageFlagBits::eSampled,
                vk::ImageAspectFlagBits::eDepth),
      mBasePass(
          {&mAlbedoTex, &mNormalTex, &mPositionTex, &mPatchIdTex, &mDepthTex},
          true),
      mGBuffer(Context::get().getDevice(), mBasePass.getHandle(),
               {
                   mAlbedoTex.getImageView(),
                   mNormalTex.getImageView(),
                   mPositionTex.getImageView(),
                   mPatchIdTex.getImageView(),
                   mDepthTex.getImageView(),
               },
               extent.width, extent.height),
      mInlineParameters({vk::ShaderStageFlagBits::eVertex}),
      mBasePassPipeline(mBasePass, extent, GeometryLayout::PositionOnly,
                        {"Source/Shaders/Build/BasePass.vert",
                         "Source/Shaders/Build/BasePass.frag"},
                        {}, mInlineParameters.getPushConstantRanges(), {}) {}

LightPassResources::LightPassResources(
    const vk::Extent2D& extent, const Texture& albedoTex,
    const Texture& normalTex, const Texture& positionTex,
    const Texture& depthTex,
    const std::vector<SamplerOptions>& depthParamOptions)
    : mConstants(vk::ShaderStageFlagBits::eFragment,
                 ShadowRenderer::sMaxLightsToRender),
      mFragmentStageParameters(
          vk::ShaderStageFlagBits::eFragment,
          ShaderParameterTexture(SamplerOptions{}.setTexture(&albedoTex)),
          ShaderParameterTexture(SamplerOptions{}.setTexture(&normalTex)),
          ShaderParameterTexture(SamplerOptions{}.setTexture(&positionTex)),
          ShaderParameterTexture(SamplerOptions{}.setTexture(&depthTex)),
          ShaderParameterArray<RTIDPRR::Component::LightProxy>(
              ShadowRenderer::sMaxLightsToRender),
          ShaderParameterTextureArray(depthParamOptions)),
      mInlineParameters({vk::ShaderStageFlagBits::eFragment}),
      mLightPassPipeline(Context::get().getSwapchain().getMainRenderPass(),
                         extent, GeometryLayout::None,
                         {"Source/Shaders/Build/LightPass.vert",
                          "Source/Shaders/Build/LightPass.frag"},
                         std::vector<vk::DescriptorSetLayout>{
                             mFragmentStageParameters.getLayout()},
                         mInlineParameters.getPushConstantRanges(),
                         generateSpecializationMap(mConstants)) {}
