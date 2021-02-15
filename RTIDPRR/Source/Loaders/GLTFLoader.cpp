#include "GLTFLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON

#include "nlohmann/json.hpp"
#include "tiny_gltf.h"

using namespace RTIDPRR;

struct GeometryData {
  std::vector<glm::vec3> vertices;
  std::vector<uint16_t> indices;
};

GeometryData processPrimitive(const tinygltf::Model& model,
                              const tinygltf::Mesh& mesh,
                              const tinygltf::Primitive& primitive) {
  const std::string positionName = "POSITION";
  const tinygltf::Accessor& positionAccessor =
      model.accessors[primitive.attributes.at(positionName)];

  std::vector<glm::vec3> positions(positionAccessor.count);
  {
    const tinygltf::BufferView& positionBufferView =
        model.bufferViews[positionAccessor.bufferView];
    const tinygltf::Buffer& positionBuffer =
        model.buffers[positionBufferView.buffer];
    const size_t positionBufferOffset =
        positionBufferView.byteOffset + positionAccessor.byteOffset;
    size_t vetexStride = positionAccessor.ByteStride(positionBufferView);
    const unsigned char* positionData =
        &positionBuffer.data[positionBufferOffset];

    const unsigned int positionCount =
        static_cast<unsigned int>(positionAccessor.count);
    for (unsigned int positionIndex = 0; positionIndex < positionCount;
         ++positionIndex) {
      const float* positionDataFloat = reinterpret_cast<const float*>(
          &positionData[vetexStride * positionIndex]);
      positions[positionIndex] = glm::vec3(
          positionDataFloat[0], positionDataFloat[1], positionDataFloat[2]);
    }
  }

  const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
  std::vector<uint16_t> indices;
  {
    const unsigned int indexCount =
        static_cast<unsigned int>(indexAccessor.count);
    const tinygltf::BufferView& indexBufferView =
        model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
    const size_t indexOffset =
        indexBufferView.byteOffset + indexAccessor.byteOffset;
    const uint16_t* pIndexData =
        reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexOffset]);
    indices = std::vector<uint16_t>(pIndexData, pIndexData + indexCount);
  }
  GeometryData geometryData{};
  geometryData.vertices = std::move(positions);
  geometryData.indices = std::move(indices);
  return geometryData;
}

void GLTFLoader::load(Scene& scene, std::string path) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  if (loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
    for (const auto& node : model.nodes) {
      Object& object = scene.addObject();
      // Load transforms
      RTIDPRR::Component::Transform& transform =
          *object.getComponent<RTIDPRR::Component::Transform>();
      if (node.translation.size() > 0) {
        transform.setLocalTranslation(glm::vec3(
            node.translation[0], node.translation[1], node.translation[2]));
      }
      if (node.rotation.size() > 0) {
        transform.setLocalRotation(glm::quat(node.rotation[3], node.rotation[0],
                                             node.rotation[1],
                                             node.rotation[2]));
      }
      if (node.scale.size() > 0) {
        transform.setLocalScale(
            glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
      }
      // Load mesh
      const int meshIndex = node.mesh;
      if (meshIndex != -1) {
        const tinygltf::Mesh& mesh = model.meshes[meshIndex];
        GeometryData data = processPrimitive(model, mesh, mesh.primitives[0]);
        object.addComponent<RTIDPRR::Component::Mesh>(data.vertices,
                                                      data.indices);
      }
    }
  }
}
