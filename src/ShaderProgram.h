#pragma once
#include <GL/glew.h>
#include <string>
#include "MathUtils.h"

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    bool Build(const std::string& vertexSrc, const std::string& fragmentSrc);
    void Release();
    void Use() const;

    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;
    void SetVec3(const char* name, const Vec3& v) const;
    void SetMat4(const char* name, const Mat4& m) const;

private:
    GLuint Compile(GLenum type, const std::string& src) const;

    GLuint program_ = 0;
};