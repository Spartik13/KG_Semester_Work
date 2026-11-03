#include "FogModel.h"

void LinearFog::ApplyUniforms(ShaderProgram& shader) const {
    shader.SetVec3("uFogColor", color_);
    shader.SetFloat("uFogStart", start_);
    shader.SetFloat("uFogEnd", end_);
}