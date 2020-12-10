#include "UUIDGenerator.h"

using namespace RTIDPRR::Random;

uint32_t UUIDGenerator::sCurrentId = 0;

uint32_t UUIDGenerator::getId() { return ++sCurrentId; }
