#include "GizmoRenderer.h"

#include <algorithm>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

static const std::vector<glm::vec3> boxVertices{
    {-1.0, -1.0, 1.0},  {1.0, -1.0, 1.0},  {1.0, 1.0, 1.0},  {-1.0, 1.0, 1.0},
    {-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}};

static const std::vector<uint16_t> boxIndices{
    0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7,
    4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};

GizmoRenderer::GizmoRenderer()
    : mInlineParameters({vk::ShaderStageFlagBits::eVertex}),
      mLinePipeline(
          Context::get().getSwapchain().getMainRenderPass(),
          Context::get().getSwapchain().getExtent(),
          GeometryLayout::PositionOnly,
          {"Source/Shaders/Build/Line.vert", "Source/Shaders/Build/Line.frag"},
          {}, mInlineParameters.getPushConstantRanges(), {},
          PipelineCreateOptions().setTopology(
              vk::PrimitiveTopology::eLineList)),
      mBoxGeometry(boxVertices, boxIndices) {
  CommandPool& commandPool = Context::get().getCommandPool();
  mCommand = commandPool.allocateCommand();
}

void GizmoRenderer::render(Scene& scene, Command* command) {
  mCommand = command;
  Swapchain& swapchain = Context::get().getSwapchain();

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  // RTIDPRR_ASSERT_VK(mCommand->begin(beginInfo));

  /* command->pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
                            vk::PipelineStageFlagBits::eTopOfPipe, {}, nullptr,
                            nullptr, nullptr);*/
  const vk::ClearColorValue clearColor(
      std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});

  const std::vector<vk::ClearValue> clearValues{clearColor};

  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Component;

  const Framebuffer& currentFramebuffer = swapchain.getCurrentFramebuffer();

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass().getHandle())
          .setFramebuffer(currentFramebuffer.getHandle())
          .setRenderArea({vk::Offset2D(0, 0),
                          vk::Extent2D(currentFramebuffer.getWidth(),
                                       currentFramebuffer.getHeight())})
          .setClearValues(clearValues);

  // mCommand->beginRenderPass(renderPassBeginInfo,
  // vk::SubpassContents::eInline);

  command->bindPipeline(mLinePipeline);

  const vk::Viewport viewport(
      0.0f, 0.0f, static_cast<float>(currentFramebuffer.getWidth()),
      static_cast<float>(currentFramebuffer.getHeight()), 0.0f, 1.0f);
  command->setViewport(0, viewport);

  using namespace RTIDPRR::Core;
  using namespace RTIDPRR::Component;

  for (const Object& object : scene.getObjects()) {
    const Transform* transform = object.getComponent<Transform>();
    if (transform) {
      object.renderGizmos(this);
    }
  }
  glm::mat4 modelViewProjection = scene.getCamera().getViewProjection();
  std::vector<CameraMatrices> matrices{
      {modelViewProjection, modelViewProjection}};
  command->pushConstants<CameraMatrices>(mLinePipeline.getPipelineLayout(),
                                         vk::ShaderStageFlagBits::eVertex, 0,
                                         matrices);
  command->bindMesh(mBoxGeometry);
  command->drawMesh(mBoxGeometry);

  command->endRenderPass();
  RTIDPRR_ASSERT_VK(command->end());
  swapchain.present(*static_cast<vk::CommandBuffer*>(command));
}

void GizmoRenderer::renderBox(const glm::mat4& viewProjectionMatrix,
                              const glm::mat4& modelMatrix) {
  glm::mat4 modelViewProjection = viewProjectionMatrix * modelMatrix;
  std::vector<CameraMatrices> matrices{{modelMatrix, modelViewProjection}};
  mCommand->pushConstants<CameraMatrices>(mLinePipeline.getPipelineLayout(),
                                          vk::ShaderStageFlagBits::eVertex, 0,
                                          matrices);
  mCommand->bindMesh(mBoxGeometry);
  mCommand->drawMesh(mBoxGeometry);
}

GizmoRenderer::~GizmoRenderer() {}
