#include "Camera.h"
#include <algorithm>

Camera::Camera() : position_(0.0f, 2.5f, 8.0f), yaw_(-90.0f), pitch_(-8.0f) {}

// Направление взгляда из углов yaw (влево-вправо) и pitch (вверх-вниз).
Vec3 Camera::Forward() const {
    float y = Radians(yaw_), p = Radians(pitch_);
    return Normalize({std::cos(p) * std::cos(y), std::sin(p), std::cos(p) * std::sin(y)});
}

Mat4 Camera::GetViewMatrix() const {
    return Mat4::LookAt(position_, position_ + Forward(), {0.0f, 1.0f, 0.0f});
}

void Camera::Rotate(float dYawDeg, float dPitchDeg) {
    yaw_ += dYawDeg;
    pitch_ = std::clamp(pitch_ + dPitchDeg, -89.0f, 89.0f);   // чтобы камера не переворачивалась
}

void Camera::MoveForward(float distance) { position_ = position_ + Forward() * distance; }

void Camera::MoveRight(float distance) {
    Vec3 right = Normalize(Cross(Forward(), {0.0f, 1.0f, 0.0f}));
    position_ = position_ + right * distance;
}

void Camera::MoveUp(float distance) { position_.y += distance; }