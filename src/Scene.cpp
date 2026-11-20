#include "Scene.h"
#include "Scene.h"
#include "SceneShaders.h"

bool Scene::InitGL() {
    if (!shader_.Build(kVertexShader, kFragmentShader)) return false;
    glEnable(GL_DEPTH_TEST);
    BuildDemoScene();
    for (auto& obj : objects_) obj->InitGL();
    return true;
}

void Scene::ReleaseGL() {
    objects_.clear();
    shader_.Release();
}

// Объекты на разном расстоянии от камеры
void Scene::BuildDemoScene() {
    objects_.clear();
    objects_.push_back(std::make_unique<Plane>(Vec3(0, 0, 0), Vec3(0.35f, 0.45f, 0.30f), 120.0f));

    // Для куба и цилиндра высота равна масштабу
    objects_.push_back(std::make_unique<Cube>(Vec3(-2.0f, 0.75f, 2.0f), Vec3(0.85f, 0.15f, 0.15f), 1.5f));
    objects_.push_back(std::make_unique<Cylinder>(Vec3(2.5f, 1.0f, -6.0f), Vec3(0.15f, 0.35f, 0.85f), 2.0f));
    objects_.push_back(std::make_unique<Cube>(Vec3(-3.5f, 1.25f, -16.0f), Vec3(0.15f, 0.70f, 0.25f), 2.5f));
    objects_.push_back(std::make_unique<Cylinder>(Vec3(4.0f, 2.5f, -26.0f), Vec3(0.90f, 0.80f, 0.15f), 5.0f));
}

void Scene::Render(int width, int height) {
    if (width < 1 || height < 1) return;
    glViewport(0, 0, width, height);

    // Фон: при включённом тумане совпадает с его цветом, чтобы дальние объекты «растворялись»
    Vec3 bg(0.55f, 0.70f, 0.85f);
    if (fogEnabled_ && fog_) bg = fog_->GetColor();
    glClearColor(bg.x, bg.y, bg.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_.Use();
    shader_.SetMat4("uView", camera_.GetViewMatrix());
    shader_.SetMat4("uProj", Mat4::Perspective(Radians(60.0f), static_cast<float>(width) / height, 0.1f, 300.0f));
    shader_.SetVec3("uLightDir", Normalize(Vec3(0.4f, 1.0f, 0.6f)));

    shader_.SetInt("uFogEnabled", (fogEnabled_ && fog_) ? 1 : 0);
    if (fog_) fog_->ApplyUniforms(shader_);   // параметры тумана передаёт сама модель

    for (const auto& obj : objects_) obj->Draw(shader_);
}