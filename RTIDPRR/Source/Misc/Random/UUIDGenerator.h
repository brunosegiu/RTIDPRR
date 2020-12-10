#pragma once

#include <cstdint>

namespace RTIDPRR {
namespace Random {
class UUIDGenerator {
 public:
  static uint32_t getId();

 private:
  static uint32_t sCurrentId;
};

}  // namespace Random
}  // namespace RTIDPRR
