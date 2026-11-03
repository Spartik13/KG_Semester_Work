#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::~ShaderProgram() { Release(); }

void ShaderProgram::Release() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

GLuint ShaderProgram::Compile(GLenum type, const std::string& src) const {
    GLuint shader = glCreateShader(type);
    if (!shader) {
        std::cerr << "Could not create shader" << std::endl;
        return 0;
    }
    const char* s = src.c_str();
    glShaderSource(shader, 1, &s, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ShaderProgram::Build(const std::string& vertexSrc, const std::string& fragmentSrc) {
    GLuint vs = Compile(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = Compile(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    // Replace the existing program only after a successful build.
    GLuint newProgram = glCreateProgram();
    if (!newProgram) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        std::cerr << "Could not create shader program" << std::endl;
        return false;
    }
    glAttachShader(newProgram, vs);
    glAttachShader(newProgram, fs);
    glLinkProgram(newProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(newProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(newProgram, sizeof(log), nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
        glDeleteProgram(newProgram);
        return false;
    }
    Release();
    program_ = newProgram;
    return true;
}

void ShaderProgram::Use() const { glUseProgram(program_); }

void ShaderProgram::SetInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(program_, name), value);
}
void ShaderProgram::SetFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(program_, name), value);
}
void ShaderProgram::SetVec3(const char* name, const Vec3& v) const {
    glUniform3f(glGetUniformLocation(program_, name), v.x, v.y, v.z);
}
void ShaderProgram::SetMat4(const char* name, const Mat4& m) const {
    glUniformMatrix4fv(glGetUniformLocation(program_, name), 1, GL_FALSE, m.m);
}
