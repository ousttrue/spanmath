#pragma once
#include "spanmath.h"
#include <cmath>

namespace spanmath {
class OrbitView {
  spanmath::Radians fovY_ = {spanmath::Degrees{30.0f}.to_rad()};
  int width_ = 1;
  int height_ = 1;
  float nearZ_ = 0.01f;
  float farZ_ = 1000.0f;

  spanmath::Degrees yaw_ = {};
  spanmath::Degrees pitch_ = {};
  float shift_[3] = {0, -0.8f, -5};

public:
  OrbitView() {}

  void SetSize(int w, int h) {
    if (w == width_ && h == height_) {
      return;
    }
    width_ = w;
    height_ = h;
  }

  void YawPitch(int dx, int dy) {
    yaw_ += {static_cast<float>(dx)};
    pitch_ += {static_cast<float>(dy)};
  }

  void Shift(int dx, int dy) {
    auto factor = std::tan(fovY_.to_rad() * 0.5f) * 2.0f * shift_[2] / height_;
    shift_[0] -= dx * factor;
    shift_[1] += dy * factor;
  }

  void Dolly(int d) {
    if (d > 0) {
      shift_[2] *= 0.9f;
    } else if (d < 0) {
      shift_[2] *= 1.1f;
    }
  }

  void Update(spanmath::Mat4 projection, spanmath::Mat4 view) const {
    float aspectRatio = (float)width_ / (float)height_;

    spanmath::make_right_handed_persepective(projection, fovY_, aspectRatio,
                                             nearZ_, farZ_);

    std::array<float, 4> _yaw;
    float Y[] = {0, 1, 0};
    auto yaw = spanmath::make_axis_angle(spanmath::Quaternion(_yaw), Y,
                                         {yaw_.to_rad()});
    std::array<float, 4> _pitch;
    float X[] = {1, 0, 0};
    auto pitch = spanmath::make_axis_angle(spanmath::Quaternion(_pitch), X,
                                           {pitch_.to_rad()});

    std::array<float, 4> _rot;
    auto rot = spanmath::make_mult(spanmath::Quaternion(_rot),
                                   spanmath::Quaternion(yaw),
                                   spanmath::Quaternion(pitch));

    spanmath::make_rigid_transform(view, rot, shift_);
  }
};
} // namespace spanmath