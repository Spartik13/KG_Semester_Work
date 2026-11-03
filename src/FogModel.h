#pragma once
#include "MathUtils.h"
#include "ShaderProgram.h"

// Абстрактная модель тумана. Наследники передают в шейдер свои параметры.
class FogModel {
public:
    explicit FogModel(const Vec3& color) : color_(color) {}
    virtual ~FogModel() = default;

    virtual void ApplyUniforms(ShaderProgram& shader) const = 0;

    const Vec3& GetColor() const { return color_; }

protected:
    Vec3 color_;
};

// Линейный туман: растёт от start до end.
class LinearFog : public FogModel {
public:
    LinearFog(const Vec3& color, float start, float end)
        : FogModel(color), start_(start), end_(end) {}
    void ApplyUniforms(ShaderProgram& shader) const override;

private:
    float start_, end_;
};