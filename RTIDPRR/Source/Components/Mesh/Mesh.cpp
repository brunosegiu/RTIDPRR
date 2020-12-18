#include "Mesh.h"

#include "../../Graphics/Loaders/GLTFLoader.h"

using namespace RTIDPRR::Component;

Mesh::Mesh(RTIDPRR::Core::Object* object, const std::string& filePath)
    : Component(object) {
  RTIDPRR::Graphics::GLTFLoader loader;
  RTIDPRR::Graphics::GLTFLoader::GeometryData data = loader.load(filePath)[0];
  mIndexedBuffer = std::make_unique<RTIDPRR::Graphics::IndexedVertexBuffer>(
      data.mVertices, data.mIndices);
}

Mesh::Mesh(Mesh&& other) noexcept
    : Component(std::move(other)),
      mIndexedBuffer(std::move(other.mIndexedBuffer)) {
  other.mIndexedBuffer = nullptr;
}

Mesh::~Mesh() {}
