#include "shaders.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdarg>
#include "logger.hpp"
#include "path_resolver.hpp"
#include "glm/ext.hpp"
#include "engine_config.hpp"

int Shader::CheckSuccess() {
    int success;
    char infoLog[512];
    glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_Shader, 512, NULL, infoLog);
        Logger::Error("SHADER::PROGRAM::LINKING_FAILED %s", infoLog);
    }
    return success;
}

void Shader::LoadSourceFromFile(std::string path) {
    std::ifstream shaderFile(path);
    if (!shaderFile.good()) {
        Logger::Error("SHADER::FILE_NOT_SUCCESSFULLY_READ: %s", path.c_str());
        return;
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    m_Source = shaderStream.str();
}

int Shader::Compile() {
    if (m_Shader == 0)
        m_Shader = glCreateShader(m_Type);
    const char* source = m_Source.c_str();
    glShaderSource(m_Shader, 1, &source, NULL);
    glCompileShader(m_Shader);
    // check for shader compile errors
    int success = CheckSuccess();
    return success;
}

Shader::Shader(ShaderType shaderType, std::string path) {
    m_Type = shaderType;
    m_Shader = 0;
    if (shaderType == VertexShader) {
        path = GetResourcePath(Resource::VSHADER, path);
    } else {
        path = GetResourcePath(Resource::FSHADER, path);
    }
    LoadSourceFromFile(path);
    Compile();
}

//             Shader Program

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
        Logger::Error("SHADER::PROGRAM::LINKING_FAILED %s", infoLog);
        m_Program = 0;
        return 1;
    }
    return 0;
}

ShaderProgram::ShaderProgram() {
    m_Program = -1;
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

int ShaderProgram::UniformLocation(const char* mode) {
    return glGetUniformLocation(m_Program, mode);
}

unsigned int ShaderProgram::GetLoc(const char* name) {
    return UniformLocation(name);
}

void ShaderProgram::SetFloat(const char* name, const float value) { glUniform1f(GetLoc(name), value); }

void ShaderProgram::SetInt(const char* name, const int value) { glUniform1i(GetLoc(name), value); }

void ShaderProgram::SetVec2(const char* name, Vec2 vec) {
    glUniform2fv(GetLoc(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::SetVec2i(const char* name, Vec2Int vec) {
    glUniform2iv(GetLoc(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::SetVec3(const char* name, Vec3 vec) {
    glUniform3fv(GetLoc(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::SetVec3i(const char* name, Vec3Int vec) {
    glUniform3iv(GetLoc(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::SetVec4(const char* name, Vec4 vec) {
    glUniform4fv(GetLoc(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::SetVec4i(const char* name, Vec4Int vec) {
    glUniform4iv(GetLoc(name), 1, glm::value_ptr(vec));
}

void ShaderProgram::SetMat3(const char* name, Mat3 mat) {
    glUniformMatrix3fv(GetLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::SetMat4(const char* name, Mat4 mat) {
    glUniformMatrix4fv(GetLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
}
