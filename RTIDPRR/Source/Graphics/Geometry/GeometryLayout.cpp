#include "GeometryLayout.h"

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

#pragma optimize("", off)
vk::VertexInputBindingDescription GeometryLayout::getBindingDescription()
    const {
  switch (mLayout) {
    case None:
      return vk::VertexInputBindingDescription{}.setInputRate(
          vk::VertexInputRate::eVertex);
      break;
    case PositionOnly: {
      return vk::VertexInputBindingDescription{}
          .setBinding(0)
          .setStride(sizeof(glm::vec3))
          .setInputRate(vk::VertexInputRate::eVertex);
    } break;
    default: {
      RTIDPRR_ASSERT_MSG(false, "No binding description found.");
      return vk::VertexInputBindingDescription();
    } break;
  }
}
#pragma optimize("", on)

vk::VertexInputAttributeDescription GeometryLayout::getAttributeDescription()
    const {
  switch (mLayout) {
    case None:
      return vk::VertexInputAttributeDescription();
      break;
    case PositionOnly: {
      static vk::VertexInputAttributeDescription attributeDescription =
          vk::VertexInputAttributeDescription()
              .setBinding(0)
              .setLocation(0)
              .setFormat(vk::Format::eR32G32B32Sfloat);
      return attributeDescription;
    } break;
    default: {
      RTIDPRR_ASSERT_MSG(false, "No vertex attribute description found.");
      return vk::VertexInputAttributeDescription();
    } break;
  }
}
