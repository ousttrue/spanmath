#pragma once
#include <span>

namespace spanmath {
struct Mat3 {
  std::span<float, 3> row0;
  std::span<float, 3> row1;
  std::span<float, 3> row2;
}
} // namespace spanmath
