#include "FogModel.h"

void LinearFog::ApplyUniforms(ShaderProgram& shader) const {
    shader.SetInt("uFogMode", 0);
    shader.SetVec3("uFogColor", color_);
    shader.SetFloat("uFogStart", start_);
    shader.SetFloat("uFogEnd", end_);

}

void ExpFog::ApplyUniforms(ShaderProgram& shader) const {
    shader.SetVec3("uFogColor", color_);
    shader.SetInt("uFogMode", 1);
    shader.SetFloat("uFogDensity", density_);
}

void Exp2Fog::ApplyUniforms(ShaderProgram& shader) const {
    shader.SetVec3("uFogColor", color_);
    shader.SetInt("uFogMode", 2);
    shader.SetFloat("uFogDensity", density_);
}