#include "MainRenderer.h"

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

static const vk::Extent2D DEFERRED_RESOLUTION = vk::Extent2D(1920, 1080);

MainRenderer::MainRenderer()
    : mShadowRenderer(),
      mBasePassRenderer(),
      mLightingPassRenderer(mShadowRenderer, mBasePassRenderer),
      mPatchCounter() {}

void MainRenderer::render(Scene& scene) {
  const Device& device = Context::get().getDevice();
  const Queue& graphicsQueue = device.getGraphicsQueue();
  Swapchain& swapchain = Context::get().getSwapchain();

  swapchain.swapBuffers();

  mShadowRenderer.render(scene);

  mBasePassRenderer.render(scene);

  mPatchCounter.count(scene, mBasePassRenderer);

  mLightingPassRenderer.render(scene, mShadowRenderer, mBasePassRenderer);

  mLightingPassRenderer.present();
}

MainRenderer::~MainRenderer() {}
