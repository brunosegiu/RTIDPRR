#include "ShadowRenderer.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

uint32_t ShadowRenderer::sMaxLightsToRender = 1;

std::vector<ShadowDepthPassResources> createLightDepthPassResources() {
  std::vector<ShadowDepthPassResources> resources;
  for (uint32_t index = 0; index < ShadowRenderer::sMaxLightsToRender;
       ++index) {
    ShadowDepthPassResources res{};
    resources.emplace_back(std::move(res));
  }
  return resources;
}

ShadowRenderer::ShadowRenderer()
    : mShadowDepthPassResources(createLightDepthPassResources()) {
  const Device& device = Context::get().getDevice();

  CommandPool& commandPool = Context::get().getCommandPool();

  mShadowDepthPassCommand = commandPool.allocateGraphicsCommand();
}

static const vk::Format depthFormat = vk::Format::eD32Sfloat;
static const vk::Format albedoFormat = vk::Format::eR8G8B8A8Unorm;
static const vk::Format patchIdFormat = vk::Format::eR32Uint;
static const uint32_t shadowTextureSize = 2048;

ShadowDepthPassResources::ShadowDepthPassResources()
    : mDepthTex(vk::Extent2D{shadowTextureSize, shadowTextureSize}, depthFormat,
                vk::ImageUsageFlagBits::eDepthStencilAttachment |
                    vk::ImageUsageFlagBits::eSampled,
                vk::ImageAspectFlagBits::eDepth),
      mAlbedoTex(vk::Extent2D{shadowTextureSize, shadowTextureSize},
                 albedoFormat,
                 vk::ImageUsageFlagBits::eColorAttachment |
                     vk::ImageUsageFlagBits::eSampled,
                 vk::ImageAspectFlagBits::eColor),
      mPatchIdTex(vk::Extent2D{shadowTextureSize, shadowTextureSize},
                  patchIdFormat,
                  vk::ImageUsageFlagBits::eColorAttachment |
                      vk::ImageUsageFlagBits::eSampled,
                  vk::ImageAspectFlagBits::eColor),
      mLightDepthRenderpass({&mAlbedoTex, &mPatchIdTex, &mDepthTex}, true),
      mLightDepthFramebuffer(
          Context::get().getDevice(), mLightDepthRenderpass.getHandle(),
          {mAlbedoTex.getImageView(), mPatchIdTex.getImageView(),
           mDepthTex.getImageView()},
          shadowTextureSize, shadowTextureSize),
      mInlineParameters({vk::ShaderStageFlagBits::eVertex}),
      mLightDepthPassPipeline(
          mLightDepthRenderpass,
          vk::Extent2D{shadowTextureSize, shadowTextureSize},
          GeometryLayout::PositionOnly,
          {"Source/Shaders/Build/LightDepthPass.vert",
           "Source/Shaders/Build/LightDepthPass.frag"},
          {}, mInlineParameters.getPushConstantRanges(), {},
          PipelineCreateOptions()
              .setCullMode(vk::CullModeFlagBits::eFront)
              .setEnableDepthBias(true)
              .setDepthBiasConstant(1.25f * 1.0f)
              .setDepthBiasSlope(1.75f * 1.0f)) {}

std::vector<SamplerOptions> ShadowRenderer::getSamplerOptionsFromResources() {
  std::vector<SamplerOptions> options;
  options.reserve(mShadowDepthPassResources.size());
  for (const ShadowDepthPassResources& resource : mShadowDepthPassResources) {
    SamplerOptions& option = options.emplace_back();
    option.setClampMode(vk::SamplerAddressMode::eClampToBorder);
    option.texture = &resource.mDepthTex;
  }
  return options;
}

void ShadowRenderer::render(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();

  const vk::ClearDepthStencilValue depthClearColor =
      vk::ClearDepthStencilValue(1.0f, 1u);
  const vk::ClearColorValue albedoClearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
  const vk::ClearColorValue patchIdClearColor(
      std::array<uint32_t, 4>{0, 0, 0, 0});

  const std::vector<vk::ClearValue> clearValues{
      albedoClearColor, patchIdClearColor, depthClearColor};

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mShadowDepthPassCommand->begin(beginInfo));

  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Component;
  LightSystem& lightSystem = scene.getSystem<LightSystem>();
  std::vector<Light>& lightComponents = lightSystem.getComponents();
  System<Mesh>& meshSystem = scene.getSystem<System<Mesh>>();

  for (uint32_t lightIndex = 0; lightIndex < ShadowRenderer::sMaxLightsToRender;
       ++lightIndex) {
    if (lightIndex > mShadowDepthPassResources.size() ||
        lightIndex > lightComponents.size()) {
      break;
    }

    ShadowDepthPassResources& resources = mShadowDepthPassResources[lightIndex];
    const Light& light = lightComponents[lightIndex];
    const LightProxy& lightProxy = light.getProxy();

    vk::RenderPassBeginInfo renderPassBeginInfo =
        vk::RenderPassBeginInfo()
            .setRenderPass(resources.mLightDepthRenderpass.getHandle())
            .setFramebuffer(resources.mLightDepthFramebuffer.getHandle())
            .setRenderArea(
                {vk::Offset2D{0, 0},
                 vk::Extent2D{resources.mLightDepthFramebuffer.getWidth(),
                              resources.mLightDepthFramebuffer.getHeight()}})
            .setClearValues(clearValues);
    mShadowDepthPassCommand->beginRenderPass(renderPassBeginInfo,
                                             vk::SubpassContents::eInline);
    mShadowDepthPassCommand->bindPipeline(resources.mLightDepthPassPipeline);

    const vk::Viewport viewport{
        0.0f,
        0.0f,
        static_cast<float>(resources.mLightDepthFramebuffer.getWidth()),
        static_cast<float>(resources.mLightDepthFramebuffer.getHeight()),
        0.0f,
        1.0f};
    mShadowDepthPassCommand->setViewport(0, viewport);
    {
      for (Mesh& mesh : meshSystem.getComponents()) {
        if (Object* obj = mesh.getObject()) {
          Transform* transform = obj->getComponent<Transform>();
          if (transform) {
            const glm::mat4 modelMatrix = transform->getAbsoluteTransform();
            glm::mat4 modelViewProjection =
                lightProxy.viewProjection * modelMatrix;

            std::vector<LightDepthPassCameraParams> matrices{
                {modelMatrix, modelViewProjection, mesh.getStartingIndex()}};

            if (light.getFrustum().intersects(modelMatrix, mesh.getAABB())) {
              mShadowDepthPassCommand
                  ->pushConstants<LightDepthPassCameraParams>(
                      resources.mLightDepthPassPipeline.getPipelineLayout(),
                      vk::ShaderStageFlagBits::eVertex, 0, matrices);
              mShadowDepthPassCommand->bindMesh(mesh.getIndexedBuffer());
              mShadowDepthPassCommand->drawMesh(mesh.getIndexedBuffer());
            }
          }
        }
      }
    }
    mShadowDepthPassCommand->endRenderPass();
  }
  RTIDPRR_ASSERT_VK(mShadowDepthPassCommand->end());
  vk::SubmitInfo submitInfo = vk::SubmitInfo().setCommandBuffers(
      *static_cast<vk::CommandBuffer*>(mShadowDepthPassCommand));
  graphicsQueue.submit(submitInfo);
}

void ShadowRenderer::transitionShadowTexturesReadOnly(Command& command) {
  std::vector<vk::ImageMemoryBarrier> barriers(
      mShadowDepthPassResources.size());
  uint32_t currentIndex = 0;
  for (vk::ImageMemoryBarrier& barrier : barriers) {
    Texture& depthTex = mShadowDepthPassResources[currentIndex].mDepthTex;
    barrier.setOldLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImage(depthTex.getImage())
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1));
    currentIndex++;
  }
  command.pipelineBarrier(vk::PipelineStageFlagBits::eLateFragmentTests,
                          vk::PipelineStageFlagBits::eFragmentShader, {},
                          nullptr, nullptr, barriers);
}

ShadowRenderer::~ShadowRenderer() {}
