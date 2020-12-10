#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Core/Scene.h"
#include "../Core/Context.h"
#include "../Core/Pipeline.h"
#include "../Core/RenderPass.h"
#include "../Core/Shaders/ShaderParameter.h"
#include "../Core/Shaders/ShaderParameterGroup.h"
#include "../Core/Shaders/ShaderParameterInlineGroup.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
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

struct BasePassResources {
  BasePassResources(const vk::Extent2D& extent);

  Texture mAlbedoTex, mNormalTex, mDepthTex;
  RenderPass mBasePass;
  Framebuffer mGBuffer;

  ShaderParameterInlineGroup<CameraMatrices> mInlineParameters;

  BasePassPipeline mBasePassPipeline;
};

struct LightPassResources {
  LightPassResources(const vk::Extent2D& extent, const Texture& albedoTex,
                     const Texture& normalTex, const Texture& depthTex);
  ShaderParameterGroup<ShaderParameterTexture, ShaderParameterTexture,
                       ShaderParameterTexture>
      mFragmentStageParameters;
  LightPassPipeline mLightPassPipeline;
};

class DeferredRenderer {
 public:
  DeferredRenderer();

  void render(Scene& scene);

  virtual ~DeferredRenderer();

 private:
  vk::CommandBuffer mCommandBuffer;

  BasePassResources mBasePassResources;
  LightPassResources mLightPassResources;

  void renderBasePass(Scene& scene);
  void renderLightPass();
};

}  // namespace Graphics
}  // namespace RTIDPRR
