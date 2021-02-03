#include "LightPassPipeline.h"

#include "../Core/Context.h"

using namespace RTIDPRR::Graphics;

LightPassPipeline::LightPassPipeline(
    const RenderPass &renderPass, const vk::Extent2D &extent,
    const std::vector<vk::DescriptorSetLayout> &descriptorLayouts,
    const std::vector<vk::PushConstantRange> &pushConstants,
    const SpecializationMap &specializationConstants)
    : Pipeline(renderPass, extent, GeometryLayout::None,
               {"Source/Shaders/Build/LightPass.vert",
                "Source/Shaders/Build/LightPass.frag"},
               descriptorLayouts, pushConstants, specializationConstants){};
