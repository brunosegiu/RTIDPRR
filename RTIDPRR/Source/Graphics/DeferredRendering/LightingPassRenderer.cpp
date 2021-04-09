#include "LightingPassRenderer.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

LightingPassRenderer::LightingPassRenderer(ShadowRenderer& shadowRenderer,
                                           BasePassRenderer& basePassRenderer)
    : mLightPassResources(Context::get().getSwapchain().getExtent(),
                          basePassRenderer.getResources().mAlbedoTex,
                          basePassRenderer.getResources().mNormalTex,
                          basePassRenderer.getResources().mPositionTex,
                          basePassRenderer.getResources().mDepthTex,
                          shadowRenderer.getSamplerOptionsFromResources()) {
  const Device& device = Context::get().getDevice();

  CommandPool& commandPool = Context::get().getCommandPool();

  mCommand = commandPool.allocateGraphicsCommand();
}

void LightingPassRenderer::render(Scene& scene, ShadowRenderer& shadowRenderer,
                                  BasePassRenderer& basePassRenderer) {
  Swapchain& swapchain = Context::get().getSwapchain();

  mCommand->beginMarker("Light Pass");

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  RTIDPRR_ASSERT_VK(mCommand->begin(beginInfo));

  shadowRenderer.transitionShadowTexturesReadOnly(*mCommand);
  basePassRenderer.transitionDepthToReadOnly(mCommand);

  mCommand->pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
                            vk::PipelineStageFlagBits::eTopOfPipe, {}, nullptr,
                            nullptr, nullptr);

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

  {
    vk::RenderPassBeginInfo renderPassBeginInfo =
        vk::RenderPassBeginInfo()
            .setRenderPass(swapchain.getMainRenderPass().getHandle())
            .setFramebuffer(currentFramebuffer.getHandle())
            .setRenderArea({vk::Offset2D(0, 0),
                            vk::Extent2D(currentFramebuffer.getWidth(),
                                         currentFramebuffer.getHeight())})
            .setClearValues(clearValues);

    mCommand->beginRenderPass(renderPassBeginInfo,
                              vk::SubpassContents::eInline);

    mCommand->bindPipeline(mLightPassResources.mLightPassPipeline);

    const vk::Viewport viewport(
        0.0f, 0.0f, static_cast<float>(currentFramebuffer.getWidth()),
        static_cast<float>(currentFramebuffer.getHeight()), 0.0f, 1.0f);
    mCommand->setViewport(0, viewport);

    mCommand->bindShaderParameterGroup(
        0, mLightPassResources.mLightPassPipeline,
        mLightPassResources.mFragmentStageParameters);

    std::vector<LightPassPushParams> matrices{
        {glm::inverse(scene.getCamera().getProjection()),
         glm::inverse(scene.getCamera().getView())}};

    mCommand->pushConstants<LightPassPushParams>(
        mLightPassResources.mLightPassPipeline.getPipelineLayout(),
        vk::ShaderStageFlagBits::eFragment, 0, matrices);

    mCommand->draw(3, 1, 0, 0);
    mCommand->endRenderPass();
  }
  mCommand->endMarker();

  RTIDPRR_ASSERT_VK(mCommand->end());
}

void LightingPassRenderer::present() {
  Swapchain& swapchain = Context::get().getSwapchain();
  swapchain.present(*mCommand);
}

LightingPassRenderer::~LightingPassRenderer() {}

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
