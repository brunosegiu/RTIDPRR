#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace RTIDPRR {
namespace Graphics {
class GLTFLoader {
 public:
  struct GeometryData {
    std::vector<glm::vec3> vertices;
    std::vector<uint16_t> indices;
  };

  GLTFLoader() = default;

  std::vector<GeometryData> load(std::string path);

  virtual ~GLTFLoader() = default;
};
}  // namespace Graphics
}  // namespace RTIDPRR
