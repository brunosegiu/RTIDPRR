#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Command.h"
#include "../Core/Pipeline.h"
#include "../Core/Shaders/ShaderParameterInlineGroup.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../Core/Shaders/ShaderParameterTextureArray.h"

namespace RTIDPRR {
namespace Graphics {

struct LightDepthPassCameraParams {
  glm::mat4 mModel;
  glm::mat4 mViewProjection;
  uint32_t mPrimitiveOffset;
};

struct ShadowDepthPassResources {
  ShadowDepthPassResources();
  ShadowDepthPassResources(ShadowDepthPassResources&& other) noexcept
      : mDepthTex(std::move(other.mDepthTex)),
        mAlbedoTex(std::move(other.mAlbedoTex)),
        mPatchIdTex(std::move(other.mPatchIdTex)),
        mLightDepthRenderpass(std::move(other.mLightDepthRenderpass)),
        mLightDepthFramebuffer(std::move(other.mLightDepthFramebuffer)),
        mInlineParameters(std::move(other.mInlineParameters)),
        mLightDepthPassPipeline(std::move(other.mLightDepthPassPipeline)) {}

  Texture mDepthTex;
  Texture mAlbedoTex;
  Texture mPatchIdTex;

  RenderPass mLightDepthRenderpass;
  Framebuffer mLightDepthFramebuffer;

  ShaderParameterInlineGroup<LightDepthPassCameraParams> mInlineParameters;

  Pipeline mLightDepthPassPipeline;

 private:
  ShadowDepthPassResources(const ShadowDepthPassResources&) = delete;
  ShadowDepthPassResources& operator=(const ShadowDepthPassResources&) = delete;
};

class ShadowRenderer {
 public:
  ShadowRenderer();

  static uint32_t sMaxLightsToRender;

  std::vector<SamplerOptions> getSamplerOptionsFromResources();

  void render(Scene& scene);

  void transitionShadowTexturesReadOnly(Command& command);

  const std::vector<ShadowDepthPassResources>& getResources() {
    return mShadowDepthPassResources;
  }

  virtual ~ShadowRenderer();

 private:
  Command* mShadowDepthPassCommand;

  std::vector<ShadowDepthPassResources> mShadowDepthPassResources;
};

}  // namespace Graphics
}  // namespace RTIDPRR
