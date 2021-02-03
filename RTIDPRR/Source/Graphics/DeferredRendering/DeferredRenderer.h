#pragma once

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
#include "../Geometry/IndexedVertexBuffer.h"
#include "BasePassPipeline.h"
#include "LightPassPipeline.h"

namespace RTIDPRR {
namespace Graphics {

struct CameraMatrices {
  glm::mat4 mModel;
  glm::mat4 mvp;
};

struct LightDepthPassCameraParams {
  glm::mat4 mModel;
  glm::mat4 mViewProjection;
};

struct LightDepthPassResources {
  LightDepthPassResources();
  LightDepthPassResources(LightDepthPassResources&& other)
      : mDepthTex(std::move(other.mDepthTex)),
        mLightDepthRenderpass(std::move(other.mLightDepthRenderpass)),
        mLightDepthFramebuffer(std::move(other.mLightDepthFramebuffer)),
        mInlineParameters(std::move(other.mInlineParameters)),
        mLightDepthPassPipeline(std::move(other.mLightDepthPassPipeline)) {}

  Texture mDepthTex;
  RenderPass mLightDepthRenderpass;
  Framebuffer mLightDepthFramebuffer;

  ShaderParameterInlineGroup<LightDepthPassCameraParams> mInlineParameters;

  Pipeline mLightDepthPassPipeline;

 private:
  LightDepthPassResources(const LightDepthPassResources&) = delete;
  LightDepthPassResources& operator=(const LightDepthPassResources&) = delete;
};

struct BasePassResources {
  BasePassResources(const vk::Extent2D& extent);

  Texture mAlbedoTex, mNormalTex, mPositionTex, mDepthTex;
  RenderPass mBasePass;
  Framebuffer mGBuffer;

  ShaderParameterInlineGroup<CameraMatrices> mInlineParameters;

  BasePassPipeline mBasePassPipeline;
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
  LightPassPipeline mLightPassPipeline;
};

class DeferredRenderer {
 public:
  DeferredRenderer();

  static uint32_t sMaxLightsToRender;

  void render(Scene& scene);

  virtual ~DeferredRenderer();

 private:
  Command *mLightDepthPassCommand, *mTransitionDepthCommand,
      *mTransitionDepthSceneCommand, *mBasePassCommand, *mLightPassCommand;

  std::vector<LightDepthPassResources> mLightDepthPassResources;
  BasePassResources mBasePassResources;
  LightPassResources mLightPassResources;

  void renderLightDepthPass(Scene& scene);
  void transitionDepthTexToReadOnly(const std::vector<Texture const*>& texture,
                                    Command* command);
  void renderBasePass(Scene& scene);
  void renderLightPass(Scene& scene);
};

}  // namespace Graphics
}  // namespace RTIDPRR
