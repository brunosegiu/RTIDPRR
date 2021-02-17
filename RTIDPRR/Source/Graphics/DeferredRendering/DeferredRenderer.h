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
#include "../GizmoRendering/GizmoRenderer.h"
#include "../ShadowRendering/ShadowRenderer.h"
#include "BasePassPipeline.h"
#include "LightPassPipeline.h"

namespace RTIDPRR {
namespace Graphics {

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

  void render(Scene& scene);

  virtual ~DeferredRenderer();

 private:
  Command *mTransitionDepthSceneCommand, *mBasePassCommand, *mLightPassCommand;

  ShadowRenderer mShadowRenderer;
  BasePassResources mBasePassResources;
  LightPassResources mLightPassResources;
  GizmoRenderer mGizmoRenderer;

  void renderBasePass(Scene& scene);
  void renderLightPass(Scene& scene);
};

}  // namespace Graphics
}  // namespace RTIDPRR
