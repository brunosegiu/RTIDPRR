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

namespace RTIDPRR {
namespace Graphics {

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

class BasePassRenderer {
 public:
  BasePassRenderer();

  void render(Scene& scene);
  void transitionDepthToReadOnly(Command* command);

  const BasePassResources& getResources() const { return mBasePassResources; }
  vk::Semaphore& getDoneSemaphore() { return mCounterWaitSemaphore; }

  virtual ~BasePassRenderer();

 private:
  Command* mCommand;

  BasePassResources mBasePassResources;
  vk::Semaphore mCounterWaitSemaphore;
};

}  // namespace Graphics
}  // namespace RTIDPRR
