#pragma once

// Вершинный шейдер: переводит вершину в экранные координаты
// и передаёт дальше позицию вершины в системе камеры. которая нужна для тумана.
inline constexpr char kVertexShader[] = R"(
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
inline constexpr char kFragmentShader[] = R"(
#version 330 core
in vec3 vViewPos;
in vec3 vNormal;
out vec4 FragColor;

uniform mat4 uView;
uniform vec3 uColor;
uniform vec3 uLightDir;

uniform int   uFogEnabled;
uniform int   uFogMode;
uniform float uFogDensity;
uniform vec3  uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

void main() {
    vec3 n = normalize(vNormal);
    vec3 l = normalize(mat3(uView) * uLightDir);
    float diff = max(dot(n, l), 0.0);
    vec3 lit = uColor * (0.35 + 0.65 * diff);
    vec3 color = lit;

    if (uFogEnabled == 1) {
        float d = length(vViewPos);
        float f;
        if (uFogMode == 1) {
            f = 1.0 - exp(-uFogDensity * d);
        } else if (uFogMode == 2) {
            float x = uFogDensity * d;
            f = 1.0 - exp(-(x * x));
        } else {
            f = clamp((d - uFogStart) / (uFogEnd - uFogStart), 0.0, 1.0);
        }
        color = mix(lit, uFogColor, f);
    }

    FragColor = vec4(color, 1.0);
}
)";