#pragma once
#include "MathUtils.h"

// Камера свободного полёта: положение + углы yaw/pitch.
class Camera {
public:
    Camera();

    Mat4 GetViewMatrix() const;
    Vec3 GetPosition() const { return position_; }

    void Rotate(float dYawDeg, float dPitchDeg);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);

private:
    Vec3 Forward() const;

    Vec3 position_;
    float yaw_;
    float pitch_;
};