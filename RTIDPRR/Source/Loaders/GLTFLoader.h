#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "../Core/Object.h"
#include "../Core/Scene.h"

namespace RTIDPRR {
class GLTFLoader {
 public:
  GLTFLoader() = default;

  void load(Scene& scene, std::string path);

  virtual ~GLTFLoader() = default;
};
}  // namespace RTIDPRR
