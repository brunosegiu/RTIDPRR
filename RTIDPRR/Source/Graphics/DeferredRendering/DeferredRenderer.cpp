#include "DeferredRenderer.h"

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

static const vk::Extent2D DEFERRED_RESOLUTION = vk::Extent2D(1920, 1080);

DeferredRenderer::DeferredRenderer()
    : mLightDepthPassResources(),
      mBasePassResources(DEFERRED_RESOLUTION),
      mLightPassResources(
          Context::get().getSwapchain().getExtent(),
          mBasePassResources.mAlbedoTex, mBasePassResources.mNormalTex,
          mBasePassResources.mPositionTex, mBasePassResources.mDepthTex,
          mLightDepthPassResources.mDepthTex) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();

  CommandPool& commandPool = Context::get().getCommandPool();

  mLightDepthPassCommand = commandPool.allocateCommand();
  mTransitionDepthCommand = commandPool.allocateCommand();
  mTransitionDepthSceneCommand = commandPool.allocateCommand();
  mBasePassCommand = commandPool.allocateCommand();
  mLightPassCommand = commandPool.allocateCommand();
}

void DeferredRenderer::render(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();

  swapchain.swapBuffers();

  renderLightDepthPass(scene);

  renderBasePass(scene);

  renderLightPass(scene);

  swapchain.submitCommand(*static_cast<vk::CommandBuffer*>(mLightPassCommand));
}

void RTIDPRR::Graphics::DeferredRenderer::renderLightDepthPass(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();

  const vk::ClearDepthStencilValue depthClearColor =
      vk::ClearDepthStencilValue(1.0f, 1u);
  const std::vector<vk::ClearValue> clearValues{depthClearColor};

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mLightDepthPassCommand->begin(beginInfo));

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(
              mLightDepthPassResources.mLightDepthRenderpass.getHandle())
          .setFramebuffer(
              mLightDepthPassResources.mLightDepthFramebuffer.getHandle())
          .setRenderArea(
              {vk::Offset2D{0, 0},
               vk::Extent2D{
                   mLightDepthPassResources.mLightDepthFramebuffer.getWidth(),
                   mLightDepthPassResources.mLightDepthFramebuffer
                       .getHeight()}})
          .setClearValues(clearValues);
  mLightDepthPassCommand->beginRenderPass(renderPassBeginInfo,
                                          vk::SubpassContents::eInline);
  mLightDepthPassCommand->bindPipeline(
      mLightDepthPassResources.mLightDepthPassPipeline);

  const vk::Viewport viewport{
      0.0f,
      0.0f,
      static_cast<float>(
          mLightDepthPassResources.mLightDepthFramebuffer.getWidth()),
      static_cast<float>(
          mLightDepthPassResources.mLightDepthFramebuffer.getHeight()),
      0.0f,
      1.0f};
  mLightDepthPassCommand->setViewport(0, viewport);
  mLightDepthPassCommand->setDepthBias(5.5f, 0.0f, 5.75f);

  {
    using namespace RTIDPRR::Core;
    using namespace RTIDPRR::Component;
    LightSystem& lightSystem = scene.getSystem<LightSystem>();

    const Light& shadedLight = lightSystem.getComponents()[0];
    LightProxy lightProxy = shadedLight.getProxy();

    System<Mesh>& meshSystem = scene.getSystem<System<Mesh>>();

    for (Mesh& mesh : meshSystem.getComponents()) {
      if (Object* obj = mesh.getObject()) {
        Transform* transform = obj->getComponent<Transform>();
        if (transform) {
          const glm::mat4 modelMatrix = transform->getAbsoluteTransform();
          glm::mat4 modelViewProjection =
              lightProxy.viewProjection * modelMatrix;

          std::vector<LightDepthPassCameraParams> matrices{
              {modelMatrix, modelViewProjection}};

          if (shadedLight.getFrustum().intersects(modelMatrix,
                                                  mesh.getAABB())) {
            mLightDepthPassCommand->pushConstants<LightDepthPassCameraParams>(
                mLightDepthPassResources.mLightDepthPassPipeline
                    .getPipelineLayout(),
                vk::ShaderStageFlagBits::eVertex, 0, matrices);
            mLightDepthPassCommand->bindMesh(mesh.getIndexedBuffer());
            mLightDepthPassCommand->drawMesh(mesh.getIndexedBuffer());
          }
        }
      }
    }
  }
  mLightDepthPassCommand->endRenderPass();
  RTIDPRR_ASSERT_VK(mLightDepthPassCommand->end());
  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(
      *static_cast<vk::CommandBuffer*>(mLightDepthPassCommand));
  graphicsQueue.submit(submitInfo);
}

void DeferredRenderer::transitionDepthTexToReadOnly(
    const std::vector<Texture*>& textures, Command* command) {
  std::vector<vk::ImageMemoryBarrier> barriers(textures.size());
  uint32_t currentIndex = 0;
  for (vk::ImageMemoryBarrier& barrier : barriers) {
    barrier.setOldLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImage(textures[currentIndex]->getImage())
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1));
    currentIndex++;
  }
  command->pipelineBarrier(vk::PipelineStageFlagBits::eLateFragmentTests,
                           vk::PipelineStageFlagBits::eFragmentShader, {},
                           nullptr, nullptr, barriers);
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
  const vk::ClearDepthStencilValue depthClearColor =
      vk::ClearDepthStencilValue(1.0f, 1u);
  const std::vector<vk::ClearValue> clearValues{
      albedoClearColor, normalClearColor, positionClearColor, depthClearColor};

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

          std::vector<CameraMatrices> matrices{
              {modelMatrix, modelViewProjection}};

          const Frustum& cameraFrustum = scene.getCamera().getFrustum();

          if (cameraFrustum.intersects(modelMatrix, mesh.getAABB())) {
            mBasePassCommand->pushConstants<CameraMatrices>(
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
  RTIDPRR_ASSERT_VK(mBasePassCommand->end());
  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(
      *static_cast<vk::CommandBuffer*>(mBasePassCommand));
  graphicsQueue.submit(submitInfo);
}

void DeferredRenderer::renderLightPass(Scene& scene) {
  Swapchain& swapchain = Context::get().getSwapchain();

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mLightPassCommand->begin(beginInfo));

  mLightPassCommand->pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
                                     vk::PipelineStageFlagBits::eTopOfPipe, {},
                                     nullptr, nullptr, nullptr);
  transitionDepthTexToReadOnly(
      std::vector<Texture*>{&mBasePassResources.mDepthTex,
                            &mLightDepthPassResources.mDepthTex},
      mLightPassCommand);

  const vk::ClearColorValue clearColor(
      std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});

  const std::vector<vk::ClearValue> clearValues{clearColor};

  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Component;

  const std::vector<Light>& lights =
      scene.getSystem<LightSystem>().getComponents();
  for (uint32_t index = 0; index < lights.size(); ++index) {
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
      mDepthTex(extent, depthFormat,
                vk::ImageUsageFlagBits::eDepthStencilAttachment |
                    vk::ImageUsageFlagBits::eSampled,
                vk::ImageAspectFlagBits::eDepth),
      mBasePass({&mAlbedoTex, &mNormalTex, &mPositionTex, &mDepthTex}, true),
      mGBuffer(Context::get().getDevice(), mBasePass.getHandle(),
               {mAlbedoTex.getImageView(), mNormalTex.getImageView(),
                mPositionTex.getImageView(), mDepthTex.getImageView()},
               extent.width, extent.height),
      mInlineParameters({vk::ShaderStageFlagBits::eVertex}),
      mBasePassPipeline(mBasePass, extent, {},
                        mInlineParameters.getPushConstantRanges()) {}

LightPassResources::LightPassResources(const vk::Extent2D& extent,
                                       const Texture& albedoTex,
                                       const Texture& normalTex,
                                       const Texture& positionTex,
                                       const Texture& depthTex,
                                       const Texture& lightDepthTex)
    : mFragmentStageParameters(
          vk::ShaderStageFlagBits::eFragment, ShaderParameterTexture(albedoTex),
          ShaderParameterTexture(normalTex),
          ShaderParameterTexture(positionTex), ShaderParameterTexture(depthTex),
          ShaderParameterArray<RTIDPRR::Component::LightProxy>(3),
          ShaderParameterTexture(lightDepthTex)),
      mInlineParameters({vk::ShaderStageFlagBits::eFragment}),
      mLightPassPipeline(Context::get().getSwapchain().getMainRenderPass(),
                         extent,
                         std::vector<vk::DescriptorSetLayout>{
                             mFragmentStageParameters.getLayout()},
                         mInlineParameters.getPushConstantRanges()) {}

LightDepthPassResources::LightDepthPassResources()
    : mDepthTex(vk::Extent2D{2048, 2048}, depthFormat,
                vk::ImageUsageFlagBits::eDepthStencilAttachment |
                    vk::ImageUsageFlagBits::eSampled,
                vk::ImageAspectFlagBits::eDepth),
      mLightDepthRenderpass({&mDepthTex}, true),
      mLightDepthFramebuffer(Context::get().getDevice(),
                             mLightDepthRenderpass.getHandle(),
                             {mDepthTex.getImageView()}, 2048, 2048),
      mInlineParameters({vk::ShaderStageFlagBits::eVertex}),
      mLightDepthPassPipeline(mLightDepthRenderpass, vk::Extent2D{2048, 2048},
                              GeometryLayout::PositionOnly,
                              {"Source/Shaders/Build/LightDepthPass.vert",
                               "Source/Shaders/Build/LightDepthPass.frag"},
                              {}, mInlineParameters.getPushConstantRanges(),
                              PipelineCreateOptions()
                                  .setCullMode(vk::CullModeFlagBits::eFront)
                                  .setEnableDepthBias(true)) {}
