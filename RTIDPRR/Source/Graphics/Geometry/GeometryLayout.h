#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace RTIDPRR {
namespace Graphics {

class GeometryLayout {
 public:
  enum Layout { None, PositionOnly };
  GeometryLayout() = default;
  constexpr GeometryLayout(Layout layout) : mLayout(layout) {}
  operator Layout() const { return mLayout; }
  explicit operator bool() = delete;

  vk::VertexInputBindingDescription getBindingDescription() const;

  vk::VertexInputAttributeDescription getAttributeDescription() const;

 private:
  Layout mLayout;
};
}  // namespace Graphics
}  // namespace RTIDPRR
