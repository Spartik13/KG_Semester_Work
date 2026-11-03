#pragma once
#include "Mesh.h"
#include "MathUtils.h"
#include "ShaderProgram.h"

// Абстрактный объект сцены. Наследники создают свою геометрию и рисуют её.
class SceneObject {
public:
    SceneObject(const Vec3& position, const Vec3& color, float scale)
        : position_(position), color_(color), scale_(scale) {}
    virtual ~SceneObject() = default;

    virtual void InitGL() = 0;
    virtual void Draw(ShaderProgram& shader) const = 0;

protected:
    void ApplyTransformAndColor(ShaderProgram& shader) const;

    Vec3 position_;
    Vec3 color_;
    float scale_;
};

class Cube : public SceneObject {
public:
    using SceneObject::SceneObject;
    void InitGL() override;
    void Draw(ShaderProgram& shader) const override;

private:
    Mesh mesh_;
};

class Cylinder : public SceneObject {
public:
    using SceneObject::SceneObject;
    void InitGL() override;
    void Draw(ShaderProgram& shader) const override;

private:
    Mesh mesh_;
};

class Plane : public SceneObject {
public:
    Plane(const Vec3& position, const Vec3& color, float halfSize)
        : SceneObject(position, color, 1.0f), halfSize_(halfSize) {}
    void InitGL() override;
    void Draw(ShaderProgram& shader) const override;

private:
    float halfSize_;
    Mesh mesh_;
};