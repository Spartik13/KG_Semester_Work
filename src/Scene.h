#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "FogModel.h"
#include "SceneObject.h"
#include "ShaderProgram.h"

// Сцена: объекты, камера, шейдер и текущая модель тумана.
class Scene {
public:
    Scene() = default;

    bool InitGL();
    void ReleaseGL();
    void Render(int width, int height);

    Camera& GetCamera() { return camera_; }

    void SetFog(std::unique_ptr<FogModel> fog) { fog_ = std::move(fog); }
    void SetFogEnabled(bool enabled) { fogEnabled_ = enabled; }

private:
    void BuildDemoScene();

    Camera camera_;
    ShaderProgram shader_;
    std::vector<std::unique_ptr<SceneObject>> objects_;
    std::unique_ptr<FogModel> fog_;   // текущая модель тумана
    bool fogEnabled_ = true;
};