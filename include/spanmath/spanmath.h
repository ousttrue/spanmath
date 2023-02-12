#pragma once
#include <DirectXMath.h>
#include <array>
#include <numbers>
#include <span>

// DirectXMath wrapper

namespace spanmath {

using ToRadians = float (*)(float value);
template <typename T, ToRadians ToRad> struct Angle {
  float value;

  Angle &operator+=(Angle rhs) {
    value += rhs.value;
    return *this;
  }

  float to_rad() const { return ToRad(value); }
};
using Radians = Angle<float, [](float value) { return value; }>;
using Degrees = Angle<float, [](float value) {
  return static_cast<float>(value * std::numbers::pi / 180.0f);
}>;

template <typename T, typename U> T cast(const U &u) {
  static_assert(sizeof(T) == sizeof(U), "same size");
  return *((T *)&u);
}

inline float dot(std::span<const float, 3> lhs, float rx, float ry, float rz) {
  return lhs[0] * rx + lhs[1] * ry + lhs[2] * rz;
}

template <typename T, size_t N> struct SpanStruct {
  using SPAN = std::span<T, N>;
  SPAN span;
  static constexpr size_t SPAN_SIZE = sizeof(T) * N;

  template <typename U>
    requires(sizeof(U) == SPAN_SIZE)
  SpanStruct(U &u) : span((T *)&u, SPAN::extent) {}

  SpanStruct(std::span<T, N> span) : span(span) {}

  template <typename T, size_t N> SpanStruct(T array[N]) : span(array, N) {}

  T &operator[](size_t i) { return span[i]; }
  const T &operator[](size_t i) const { return span[i]; }

  template <typename U>
    requires(sizeof(U) == SPAN_SIZE)
  U *cast_data() {
    return (U *)span.data();
  }
};

///
/// * row major memory layout
///
/// [0, 1, 2][x]    [0x + 1y + 2z]
/// [3, 4, 5][y] => [3x + 4y + 5z]
/// [6, 7, 8][z]    [6x + 7y + 8z]
///
struct Mat3 : public SpanStruct<float, 9> {

  std::span<float, 3> row0() { return std::span<float, 3>(span.data(), 3); }
  std::span<float, 3> row1() { return std::span<float, 3>(span.data() + 3, 3); }
  std::span<float, 3> row2() { return std::span<float, 3>(span.data() + 6, 3); }

  float &operator[](size_t index) const { return span[index]; }

  std::array<float, 9> operator*(Mat3 rhs) {
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

struct Quaternion : public SpanStruct<float, 4> {
  DirectX::XMVECTOR load_dx() {
    return DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)span.data());
  }
  void store_dx(DirectX::XMVECTOR q) {
    DirectX::XMStoreFloat4((DirectX::XMFLOAT4 *)span.data(), q);
  }
};

inline Quaternion make_mult(Quaternion dst, Quaternion lhs, Quaternion rhs) {
  DirectX::XMStoreFloat4(
      dst.cast_data<DirectX::XMFLOAT4>(),
      DirectX::XMQuaternionMultiply(lhs.load_dx(), rhs.load_dx()));
  return dst;
}
inline Quaternion
make_axis_angle(Quaternion dst, std::span<const float, 3> axis, Radians angle) {
  dst.store_dx(DirectX::XMQuaternionRotationAxis(
      DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)axis.data()), angle.to_rad()));
  return dst;
}

struct Mat4 : public SpanStruct<float, 16> {
  std::span<float, 4> row0() { return std::span<float, 4>(span.data(), 4); }
  std::span<float, 4> row1() { return std::span<float, 4>(span.data() + 4, 4); }
  std::span<float, 4> row2() { return std::span<float, 4>(span.data() + 8, 4); }
  std::span<float, 4> row3() {
    return std::span<float, 4>(span.data() + 12, 4);
  }
  DirectX::XMMATRIX load_dx() {
    return DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)span.data());
  }
  void store_dx(DirectX::XMMATRIX m) {
    DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4 *)span.data(), m);
  }
};

inline Mat4 make_right_handed_persepective(Mat4 dst, Radians fovY,
                                           float aspectRatio, float nearZ,
                                           float farZ) {
  dst.store_dx(DirectX::XMMatrixPerspectiveFovRH(fovY.to_rad(), aspectRatio,
                                                 nearZ, farZ));
  return dst;
}

inline Mat4 make_rigid_transform(Mat4 dst, Quaternion rotation,
                                 std::span<const float, 3> translation) {

  auto r = rotation.load_dx();
  auto m = DirectX::XMMatrixRotationQuaternion(r);
  dst.store_dx(m);
  auto t = dst.row3();
  t[0] = translation[0];
  t[1] = translation[1];
  t[2] = translation[2];
  return dst;
}

} // namespace spanmath
