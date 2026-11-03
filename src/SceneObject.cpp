#include "SceneObject.h"

// Объекты, которые используются в сцене: кубы, цилиндры, пол
void SceneObject::ApplyTransformAndColor(ShaderProgram& shader) const {
    shader.SetMat4("uModel", Mat4::TranslateScale(position_, scale_));
    shader.SetVec3("uColor", color_);
}

void Cube::InitGL() { mesh_.Create(MakeCubeData()); }
void Cube::Draw(ShaderProgram& shader) const {
    ApplyTransformAndColor(shader);
    mesh_.Draw();
}

void Cylinder::InitGL() { mesh_.Create(MakeCylinderData(32)); }
void Cylinder::Draw(ShaderProgram& shader) const {
    ApplyTransformAndColor(shader);
    mesh_.Draw();
}

void Plane::InitGL() { mesh_.Create(MakePlaneData(halfSize_)); }
void Plane::Draw(ShaderProgram& shader) const {
    ApplyTransformAndColor(shader);
    mesh_.Draw();
}