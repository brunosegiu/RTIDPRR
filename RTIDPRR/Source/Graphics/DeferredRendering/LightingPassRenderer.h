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
#include "../CountPatches/PatchCounter.h"
#include "../Geometry/IndexedVertexBuffer.h"
#include "../ShadowRendering/ShadowRenderer.h"
#include "BasePassRenderer.h"

namespace RTIDPRR {
namespace Graphics {

struct LightPassPushParams {
  glm::mat4 mModel;
  glm::mat4 mvp;
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
  ShaderParameterInlineGroup<LightPassPushParams> mInlineParameters;
  Pipeline mLightPassPipeline;
};

class LightingPassRenderer {
 public:
  LightingPassRenderer(ShadowRenderer& shadowRenderer,
                       BasePassRenderer& basePassRenderer);

  void render(Scene& scene, ShadowRenderer& shadowRenderer,
              BasePassRenderer& basePassRenderer);

  void present();

  virtual ~LightingPassRenderer();

 private:
  Command* mCommand;

  LightPassResources mLightPassResources;
};

}  // namespace Graphics
}  // namespace RTIDPRR
