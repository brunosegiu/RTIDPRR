#include "DeferredRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Loaders/GLTFLoader.h"
using namespace RTIDPRR::Graphics;

DeferredRenderer::DeferredRenderer()
    : mShaderParameters(vk::ShaderStageFlagBits::eVertex),
      mPipeline(
          Context::get().getSwapchain().getMainRenderPass(),
          *Shader::loadShader("Source/Shaders/Build/test.vert"),
          *Shader::loadShader("Source/Shaders/Build/test.frag"),
          std::vector<vk::DescriptorSetLayout>{mShaderParameters.getLayout()}) {
  GLTFLoader loader;
  GLTFLoader::GeometryData data = loader.load("Assets/Models/monkey.glb")[0];
  mMesh = std::make_unique<IndexedVertexBuffer>(data.mVertices, data.mIndices);
}

void DeferredRenderer::render() {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();
  swapchain.swapBuffers();
  glm::mat4 proj =
      glm::perspective(glm::radians(70.0f), 1280.0f / 720.0f, 0.1f, 10.0f);
  proj[1][1] *= -1.0f;
  glm::mat4 view =
      glm::lookAt(glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  static float time = 0.0f;
  time += 0.01f;
  glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 viewProj = proj * view * model;
  std::get<0>(mShaderParameters.mParameters).update(viewProj);

  vk::CommandBufferAllocateInfo commandAllocInfo =
      vk::CommandBufferAllocateInfo()
          .setCommandPool(graphicsQueue.getCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1);

  std::vector<vk::CommandBuffer> commandBuffers =
      device.getLogicalDevice().allocateCommandBuffers(commandAllocInfo);

  vk::CommandBuffer& currentCommandBuffer = commandBuffers[0];

  vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
  currentCommandBuffer.begin(beginInfo);

  const vk::ClearColorValue clearColor(
      std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});

  const std::vector<vk::ClearValue> clearValues{clearColor};

  vk::RenderPassBeginInfo renderPassBeginInfo =
      vk::RenderPassBeginInfo()
          .setRenderPass(swapchain.getMainRenderPass())
          .setFramebuffer(
              swapchain.getCurrentFramebuffer().getFramebufferHandle())
          .setRenderArea({vk::Offset2D{0, 0}, vk::Extent2D{1280, 720}})
          .setClearValues(clearValues);

  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    mPipeline.getPipelineHandle());

  currentCommandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, mPipeline.getPipelineLayout(), 0,
      mShaderParameters.getDescriptorSet(), nullptr);

  mMesh->draw(currentCommandBuffer);

  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();

  swapchain.submitCommand(currentCommandBuffer);
}

DeferredRenderer::~DeferredRenderer() {}
