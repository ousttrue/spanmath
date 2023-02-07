#pragma once
#include <array>
#include <span>

namespace spanmath {

///
/// * row major memory layout
///
/// [0, 1, 2][x]    [0x + 1y + 2z]
/// [3, 4, 5][y] => [3x + 4y + 5z]
/// [6, 7, 8][z]    [6x + 7y + 8z]
///

inline float dot(std::span<const float, 3> lhs, float rx, float ry, float rz) {
  auto [lx, ly, lz] = *((std::tuple<float, float, float> *)lhs.data());
  return lx * rx + ly * ry + lz * rz;
}

struct Mat3 {
  std::span<float, 9> span;

  template <typename T> 
  Mat3(T &t):span((float *)&t, 9) {
    static_assert(sizeof(T) == sizeof(float) * 9);
  }

  std::span<const float, 3> row0() const {
    return std::span<const float, 3>(span.data(), 3);
  }
  std::span<const float, 3> row1() const {
    return std::span<const float, 3>(span.data() + 3, 3);
  }
  std::span<const float, 3> row2() const {
    return std::span<const float, 3>(span.data() + 6, 3);
  }

  float &operator[](size_t index) const { return span[index]; }

  std::array<float, 9> operator*(const Mat3 &rhs) const {
    return {dot(row0(), rhs[0], rhs[3], rhs[6]),
            dot(row0(), rhs[1], rhs[4], rhs[7]),
            dot(row0(), rhs[2], rhs[5], rhs[8]),
            dot(row1(), rhs[0], rhs[3], rhs[6]),
            dot(row1(), rhs[1], rhs[4], rhs[7]),
            dot(row1(), rhs[2], rhs[5], rhs[8]),
            dot(row2(), rhs[0], rhs[3], rhs[6]),
            dot(row2(), rhs[1], rhs[4], rhs[7]),
            dot(row2(), rhs[2], rhs[5], rhs[8])};
  }
};

} // namespace spanmath
