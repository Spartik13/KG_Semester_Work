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

// Экспоненциальный туман. Доля цвета тумана растёт с расстоянием
// по формуле f = 1 - exp(-density * distance).
class ExpFog : public FogModel {
public:
    ExpFog(const Vec3& color, float density)
        : FogModel(color), density_(density) {}

    void ApplyUniforms(ShaderProgram& shader) const override;

private:
    float density_;
};

// Квадратичный экспоненциальный туман. В показателе степени
// используется квадрат произведения плотности и расстояния.
class Exp2Fog : public FogModel {
public:
    Exp2Fog(const Vec3& color, float density)
        : FogModel(color), density_(density) {}

    void ApplyUniforms(ShaderProgram& shader) const override;

private:
    float density_;
};