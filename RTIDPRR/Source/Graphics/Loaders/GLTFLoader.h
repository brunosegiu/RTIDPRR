#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace RTIDPRR {
namespace Graphics {
class GLTFLoader {
 public:
  struct GeometryData {
    std::vector<glm::vec3> mVertices;
    std::vector<uint16_t> mIndices;
  };

  GLTFLoader() = default;

  std::vector<GeometryData> load(std::string path);

  virtual ~GLTFLoader() = default;
};
}  // namespace Graphics
}  // namespace RTIDPRR
