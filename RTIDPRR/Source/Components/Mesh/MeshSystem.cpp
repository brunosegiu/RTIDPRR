#include "MeshSystem.h"

#include "../../Core/Scene.h"
#include "../../Core/System.h"

using namespace RTIDPRR::Component;

MeshSystem::MeshSystem() : RTIDPRR::Core::System<Mesh>() {}

void MeshSystem::update(const float deltaTime) {}

MeshSystem::~MeshSystem() {}
