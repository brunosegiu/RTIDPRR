#include "ShadowRenderer.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

uint32_t ShadowRenderer::sMaxLightsToRender = 2;

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
    : mShadowDepthPassResources(std::move(createLightDepthPassResources())) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();

  CommandPool& commandPool = Context::get().getCommandPool();

  mShadowDepthPassCommand = commandPool.allocateCommand();
}

static const vk::Format depthFormat = vk::Format::eD32Sfloat;

ShadowDepthPassResources::ShadowDepthPassResources()
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
                              {}, mInlineParameters.getPushConstantRanges(), {},
                              PipelineCreateOptions()
                                  .setCullMode(vk::CullModeFlagBits::eFront)
                                  .setEnableDepthBias(true)) {}

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
  const std::vector<vk::ClearValue> clearValues{depthClearColor};
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
    mShadowDepthPassCommand->setDepthBias(1.25f * 5.0f, 0.0f, 1.75f * 7.0f);
    {
      for (Mesh& mesh : meshSystem.getComponents()) {
        if (Object* obj = mesh.getObject()) {
          Transform* transform = obj->getComponent<Transform>();
          if (transform) {
            const glm::mat4 modelMatrix = transform->getAbsoluteTransform();
            glm::mat4 modelViewProjection =
                lightProxy.viewProjection * modelMatrix;

            std::vector<LightDepthPassCameraParams> matrices{
                {modelMatrix, modelViewProjection}};

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
