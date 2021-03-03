﻿#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Command.h"
#include "../Core/Context.h"
#include "../Core/Pipeline.h"
#include "../Core/RenderPass.h"
#include "../Core/Shaders/ShaderParameter.h"
#include "../Core/Shaders/ShaderParameterArray.h"
#include "../Core/Shaders/ShaderParameterConstantGroup.h"
#include "../Core/Shaders/ShaderParameterGroup.h"
#include "../Core/Shaders/ShaderParameterInlineGroup.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../Core/Shaders/ShaderParameterTextureArray.h"
#include "../Core/Texture.h"
#include "../CountPatches/PatchCounter.h"
#include "../Geometry/IndexedVertexBuffer.h"
#include "../ShadowRendering/ShadowRenderer.h"

namespace RTIDPRR {
namespace Graphics {

struct CameraMatrices {
  glm::mat4 mModel;
  glm::mat4 mvp;
};

struct BasePassPushParams {
  glm::mat4 mModel;
  glm::mat4 mvp;
  uint32_t mStartingIndex;
};

struct BasePassResources {
  BasePassResources(const vk::Extent2D& extent);

  Texture mAlbedoTex, mNormalTex, mPositionTex, mPatchIdTex, mDepthTex;
  RenderPass mBasePass;
  Framebuffer mGBuffer;

  ShaderParameterInlineGroup<BasePassPushParams> mInlineParameters;

  Pipeline mBasePassPipeline;
};

struct LightPassResources {
  LightPassResources(const vk::Extent2D& extent, const Texture& albedoTex,
                     const Texture& normalTex, const Texture& positionTex,
                     const Texture& depthTex,
                     const std::vector<SamplerOptions>& depthParamOptions);
  ShaderParameterConstantGroup<uint32_t> mConstants;
  ShaderParameterGroup<ShaderParameterTexture, ShaderParameterTexture,
                       ShaderParameterTexture, ShaderParameterTexture,
                       ShaderParameterArray<RTIDPRR::Component::LightProxy>,
                       ShaderParameterTextureArray>
      mFragmentStageParameters;
  ShaderParameterInlineGroup<CameraMatrices> mInlineParameters;
  Pipeline mLightPassPipeline;
};

class DeferredRenderer {
 public:
  DeferredRenderer();

  void render(Scene& scene);

  virtual ~DeferredRenderer();

 private:
  Command *mTransitionDepthSceneCommand, *mBasePassCommand, *mLightPassCommand;

  ShadowRenderer mShadowRenderer;
  BasePassResources mBasePassResources;
  LightPassResources mLightPassResources;
  PatchCounter mPatchCounter;
  vk::Semaphore mCounterWaitSemaphore;

  void renderBasePass(Scene& scene);
  void renderLightPass(Scene& scene);
};

}  // namespace Graphics
}  // namespace RTIDPRR
