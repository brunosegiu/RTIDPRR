#include "BasePassRenderer.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

static const vk::Extent2D DEFERRED_RESOLUTION = vk::Extent2D(1920, 1080);

BasePassRenderer::BasePassRenderer() : mBasePassResources(DEFERRED_RESOLUTION) {
  const Device& device = Context::get().getDevice();
  CommandPool& commandPool = Context::get().getCommandPool();
  mCommand = commandPool.allocateGraphicsCommand();
  mCounterWaitSemaphore =
      RTIDPRR_ASSERT_VK(device.getLogicalDeviceHandle().createSemaphore(
          vk::SemaphoreCreateInfo()));
}

void BasePassRenderer::render(Scene& scene) {
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
  RTIDPRR_ASSERT_VK(mCommand->begin(beginInfo));

  mCommand->beginMarker("Base Pass");
  {
    vk::RenderPassBeginInfo renderPassBeginInfo =
        vk::RenderPassBeginInfo()
            .setRenderPass(mBasePassResources.mBasePass.getHandle())
            .setFramebuffer(mBasePassResources.mGBuffer.getHandle())
            .setRenderArea({vk::Offset2D{0, 0}, DEFERRED_RESOLUTION})
            .setClearValues(clearValues);
    mCommand->beginRenderPass(renderPassBeginInfo,
                              vk::SubpassContents::eInline);
    mCommand->bindPipeline(mBasePassResources.mBasePassPipeline);

    const vk::Viewport viewport{
        0.0f,
        0.0f,
        static_cast<float>(mBasePassResources.mGBuffer.getWidth()),
        static_cast<float>(mBasePassResources.mGBuffer.getHeight()),
        0.0f,
        1.0f};
    mCommand->setViewport(0, viewport);
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
              mCommand->pushConstants<BasePassPushParams>(
                  mBasePassResources.mBasePassPipeline.getPipelineLayout(),
                  vk::ShaderStageFlagBits::eVertex, 0, matrices);
              mCommand->bindMesh(mesh.getIndexedBuffer());
              mCommand->drawMesh(mesh.getIndexedBuffer());
            }
          }
        }
      }
    }

    mCommand->endRenderPass();
  }
  mCommand->endMarker();

  /*mCommand->transitionTexture(ResourceTransition::GraphicsToCompute,
                              vk::PipelineStageFlagBits::eColorAttachmentOutput,
                              vk::PipelineStageFlagBits::eBottomOfPipe,
                              vk::ImageLayout::eShaderReadOnlyOptimal,
                              vk::ImageLayout::eShaderReadOnlyOptimal, {},
                              &mBasePassResources.mPatchIdTex);*/

  RTIDPRR_ASSERT_VK(mCommand->end());
  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(
      *static_cast<vk::CommandBuffer*>(mCommand));

  //.setSignalSemaphores(mCounterWaitSemaphore);
  graphicsQueue.submit(submitInfo);
}

void BasePassRenderer::transitionDepthToReadOnly(Command* command) {
  command->transitionTexture(ResourceTransition::GraphicsToGraphics,
                             vk::PipelineStageFlagBits::eLateFragmentTests,
                             vk::PipelineStageFlagBits::eFragmentShader,
                             vk::ImageLayout::eDepthStencilAttachmentOptimal,
                             vk::ImageLayout::eShaderReadOnlyOptimal, {},
                             &mBasePassResources.mDepthTex);
}

BasePassRenderer::~BasePassRenderer() {
  Context::get().getDevice().getLogicalDeviceHandle().destroySemaphore(
      mCounterWaitSemaphore);
}

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
