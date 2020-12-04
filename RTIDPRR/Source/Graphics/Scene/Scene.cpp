#include "Scene.h"

#include "../Loaders/GLTFLoader.h"

using namespace RTIDPRR::Graphics;

Scene::Scene() : mCamera() {
  GLTFLoader loader;
  GLTFLoader::GeometryData data = loader.load("Assets/Models/monkey.glb")[0];
  mMeshes.emplace_back(data.mVertices, data.mIndices);
}

Scene::~Scene() {}
