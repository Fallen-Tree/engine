#include "shaders.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <iostream>
#include <cstdarg>

#include <glm/glm.hpp>

int ShaderProgram::AttachShader(Shader shader) {
    glAttachShader(m_Program, shader.m_Shader);
    return 0;
}

int ShaderProgram::Link() {
    glLinkProgram(m_Program);
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        m_Program = 0;
        return 1;
    }
    return 0;
}

ShaderProgram::ShaderProgram() {
    m_Program = 0;
}

ShaderProgram::ShaderProgram(Shader vShader, Shader fShader) {
    m_Program = glCreateProgram();
    AttachShader(vShader);
    AttachShader(fShader);
    Link();
}

int ShaderProgram::Use() {
    glUseProgram(m_Program);
    return 0;
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_Program);
}

int ShaderProgram::UniformLocation(const char* mode) {
    return glGetUniformLocation(m_Program, mode);
}

void ShaderProgram::SetValue(const char* name, const float value) {
    int loc = UniformLocation(name);
    glUniform1f(loc, value);
}

void ShaderProgram::SetVec3(const char* name, const float* vec) {
    int loc = UniformLocation(name);
    glUniform3fv(loc, 1, vec);
}

void ShaderProgram::SetVecs3(const char* name, unsigned count, const float* vecs) {
    int loc = UniformLocation(name);
    glUniform3fv(loc, count, vecs);
}

void ShaderProgram::SetMat4(const char* name, const float* mat) {
    int loc = UniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void ShaderProgram::SetMats4(const char* name, unsigned count, const float* mat) {
    int loc = UniformLocation(name);
    glUniformMatrix4fv(loc, count, GL_FALSE, mat);
}
