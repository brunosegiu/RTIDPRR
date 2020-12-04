#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../Core/Context.h"
#include "../Core/Pipeline.h"
#include "../Core/RenderPass.h"
#include "../Core/Shaders/ShaderParameterGroup.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../Core/Texture.h"
#include "../Geometry/IndexedVertexBuffer.h"
#include "../Scene/Scene.h"
#include "BasePassPipeline.h"
#include "LightPassPipeline.h"

namespace RTIDPRR {
namespace Graphics {

struct BasePassResources {
  BasePassResources(const vk::Extent2D& extent);

  Texture mAlbedoTex, mNormalTex, mDepthTex;
  RenderPass mBasePass;
  Framebuffer mGBuffer;

  ShaderParameterGroup<ShaderParameter<glm::mat4>> mVertexStageParameters;

  BasePassPipeline mBasePassPipeline;
};

struct LightPassResources {
  LightPassResources(const vk::Extent2D& extent, const Texture& albedoTex,
                     const Texture& normalTex);
  RenderPass mLightPass;
  ShaderParameterGroup<ShaderParameterTexture, ShaderParameterTexture>
      mFragmentStageParameters;
  LightPassPipeline mLightPassPipeline;
};

class DeferredRenderer {
 public:
  DeferredRenderer();

  void render(const Scene& scene);

  virtual ~DeferredRenderer();

 private:
  vk::CommandBuffer mCommandBuffer;

  BasePassResources mBasePassResources;
  LightPassResources mLightPassResources;

  void renderBasePass(const Scene& scene);
  void renderLightPass();
};

}  // namespace Graphics
}  // namespace RTIDPRR
