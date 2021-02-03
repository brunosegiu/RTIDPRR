#include "BasePassPipeline.h"

#include "../Core/Context.h"

using namespace RTIDPRR::Graphics;

BasePassPipeline::BasePassPipeline(
    const RenderPass &renderPass, const vk::Extent2D &extent,
    const std::vector<vk::DescriptorSetLayout> &descriptorLayouts,
    const std::vector<vk::PushConstantRange> &pushConstants)
    : Pipeline(renderPass, extent, GeometryLayout::PositionOnly,
               {"Source/Shaders/Build/BasePass.vert",
                "Source/Shaders/Build/BasePass.frag"},
               descriptorLayouts, pushConstants, {}){};
