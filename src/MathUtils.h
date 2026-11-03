#pragma once
#include <cmath>

// Небольшая математическая библиотека: векторы и матрицы 4x4.

struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
};

inline float Dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 Cross(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
inline Vec3 Normalize(const Vec3& v) {
    float len = std::sqrt(Dot(v, v));
    return len > 0 ? v * (1.0f / len) : v;
}

struct Mat4 {
    float m[16];

    static Mat4 Identity() {
        Mat4 r{};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    // Матрица модели: равномерное масштабирование и перенос.
    static Mat4 TranslateScale(const Vec3& pos, float scale) {
        Mat4 r = Identity();
        r.m[0] = r.m[5] = r.m[10] = scale;
        r.m[12] = pos.x;
        r.m[13] = pos.y;
        r.m[14] = pos.z;
        return r;
    }

    static Mat4 Perspective(float fovyRad, float aspect, float zNear, float zFar) {
        Mat4 r{};
        float f = 1.0f / std::tan(fovyRad / 2.0f);
        r.m[0] = f / aspect;
        r.m[5] = f;
        r.m[10] = (zFar + zNear) / (zNear - zFar);
        r.m[11] = -1.0f;
        r.m[14] = 2.0f * zFar * zNear / (zNear - zFar);
        return r;
    }

    static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = Normalize(center - eye);
        Vec3 s = Normalize(Cross(f, up));
        Vec3 u = Cross(s, f);
        Mat4 r = Identity();
        r.m[0] = s.x;  r.m[4] = s.y;  r.m[8] = s.z;
        r.m[1] = u.x;  r.m[5] = u.y;  r.m[9] = u.z;
        r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
        r.m[12] = -Dot(s, eye);
        r.m[13] = -Dot(u, eye);
        r.m[14] = Dot(f, eye);
        return r;
    }
};

constexpr float kPi = 3.14159265358979f;
inline float Radians(float deg) { return deg * kPi / 180.0f; }