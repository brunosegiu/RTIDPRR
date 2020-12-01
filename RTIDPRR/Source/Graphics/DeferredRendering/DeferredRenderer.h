#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../Core/Context.h"
#include "../Core/Pipeline.h"
#include "../Core/Shaders/ShaderParameterGroup.h"
#include "../Core/Shaders/ShaderParameterTexture.h"
#include "../Core/Texture.h"
#include "../Geometry/IndexedVertexBuffer.h"
#include "BasePassPipeline.h"

namespace RTIDPRR {
namespace Graphics {

class BasePassResources {
 public:
  BasePassResources(const vk::Extent2D& extent);

  vk::RenderPass mBasePass;

  Texture mAlbedoTex, mNormalTex, mDepthTex;

  ShaderParameterGroup<ShaderParameter<glm::mat4>> mVertexStageParameters;

  Framebuffer mGBuffer;

  BasePassPipeline mBasePassPipeline;
};

class DeferredRenderer {
 public:
  DeferredRenderer();

  void render();

  virtual ~DeferredRenderer();

 private:
  vk::CommandBuffer mCommandBuffer;

  std::unique_ptr<IndexedVertexBuffer> mMesh;

  BasePassResources mBasePassResources;

  ShaderParameterGroup<ShaderParameterTexture, ShaderParameterTexture>
      mFragmentStageParameters;
  Pipeline mPipeline;

  void renderBasePass();
};

}  // namespace Graphics
}  // namespace RTIDPRR
