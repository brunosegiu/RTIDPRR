#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "../../Misc/DebugUtils.h"

namespace RTIDPRR {
namespace Graphics {

class GeometryLayout {
 public:
  enum Layout { None, PositionOnly };
  GeometryLayout() = default;
  constexpr GeometryLayout(Layout layout) : mLayout(layout) {}
  operator Layout() const { return mLayout; }
  explicit operator bool() = delete;

  vk::VertexInputBindingDescription getBindingDescription(
      const GeometryLayout& layout) const {
    switch (layout) {
      case None:
        return vk::VertexInputBindingDescription();
        break;
      case PositionOnly: {
        static vk::VertexInputBindingDescription vertexInput =
            vk::VertexInputBindingDescription()
                .setBinding(0)
                .setStride(sizeof(glm::vec3))
                .setInputRate(vk::VertexInputRate::eVertex);
        return vertexInput;
      } break;
      default: {
        RTIDPRR_ASSERT_MSG(false, "No binding description found.");
        return vk::VertexInputBindingDescription();
      } break;
    }
  }

  vk::VertexInputAttributeDescription getAttributeDescription(
      const GeometryLayout& layout) const {
    switch (layout) {
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

 private:
  Layout mLayout;
};
}  // namespace Graphics
}  // namespace RTIDPRR
