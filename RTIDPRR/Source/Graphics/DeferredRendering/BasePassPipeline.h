﻿#pragma once

#include <vulkan/vulkan.hpp>

#include "../Core/Pipeline.h"
#include "../Core/Shaders/Shader.h"

namespace RTIDPRR {
namespace Graphics {
class BasePassPipeline : public Pipeline {
 public:
  BasePassPipeline(
      const RenderPass& renderPass, const vk::Extent2D& extent,
      const std::vector<vk::DescriptorSetLayout>& descriptorLayouts,
      const std::vector<vk::PushConstantRange>& pushConstants);

  virtual ~BasePassPipeline() = default;
};
}  // namespace Graphics
}  // namespace RTIDPRR
