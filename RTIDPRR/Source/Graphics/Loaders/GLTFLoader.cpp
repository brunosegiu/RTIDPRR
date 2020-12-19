#include "GLTFLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON

#include "nlohmann/json.hpp"
#include "tiny_gltf.h"

using namespace RTIDPRR::Graphics;

GLTFLoader::GeometryData processPrimitive(
    const tinygltf::Model& model, const tinygltf::Mesh& mesh,
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
  GLTFLoader::GeometryData geometryData{};
  geometryData.vertices = std::move(positions);
  geometryData.indices = std::move(indices);
  return geometryData;
}

std::vector<GLTFLoader::GeometryData> GLTFLoader::load(std::string path) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  std::vector<GLTFLoader::GeometryData> entities;
  entities.reserve(model.meshes.size());

  if (loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
    for (const auto& mesh : model.meshes) {
      for (const auto& primitive : mesh.primitives) {
        entities.emplace_back(processPrimitive(model, mesh, primitive));
      }
    }
  }
  return entities;
}
