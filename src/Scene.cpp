#include "Scene.h"

namespace {

// Вершинный шейдер: переводит вершину в экранные координаты
// и передаёт дальше позицию вершины в системе камеры. которая нужна для тумана.
const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vViewPos;
out vec3 vNormal;

void main() {
    vec4 viewPos = uView * uModel * vec4(aPos, 1.0);
    vViewPos = viewPos.xyz;
    vNormal = mat3(uView * uModel) * aNormal;
    gl_Position = uProj * viewPos;
}
)";

// Фрагментный шейдер: цвет объекта с освещением и расчет тумана.
// f — степень тумана: 0 = объект виден полностью, 1 = полностью скрыт.
const char* kFragmentShader = R"(
#version 330 core
in vec3 vViewPos;
in vec3 vNormal;
out vec4 FragColor;

uniform mat4 uView;
uniform vec3 uColor;
uniform vec3 uLightDir;

uniform int   uFogEnabled;
uniform vec3  uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

void main() {
    vec3 n = normalize(vNormal);
    vec3 l = normalize(mat3(uView) * uLightDir);
    float diff = max(dot(n, l), 0.0);
    vec3 lit = uColor * (0.35 + 0.65 * diff);

    float d = length(vViewPos);
    // линейный туман: 0 на дистанции start, 1 на дистанции end
    float f = clamp((d - uFogStart) / (uFogEnd - uFogStart), 0.0, 1.0);

    vec3 color = (uFogEnabled == 1) ? mix(lit, uFogColor, f) : lit;
    FragColor = vec4(color, 1.0);
}
)";

}  // namespace

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