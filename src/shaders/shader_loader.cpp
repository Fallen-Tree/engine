
#include "shader_loader.hpp"
#include "../logging/logger.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdarg>

#include <glm/glm.hpp>

int Shader::CheckSuccess() {
    int success;
    char infoLog[512];
    glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_Shader, 512, NULL, infoLog);
        Logger::Error("ERROR::SHADER::PROGRAM::LINKING_FAILED %s", infoLog);
    }
    return success;
}

int Shader::LoadSourceFromFile(const char* path) {
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file's buffer contents into stream
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        m_Source = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        Logger::Error("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: %s", e.what());
    }
    return 0;
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

Shader::Shader(ShaderType shaderType, const char* path) {
    m_Type = shaderType;
    m_Shader = 0;
    LoadSourceFromFile(path);
    Compile();
}

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
        Logger::Error("ERROR::SHADER::PROGRAM::LINKING_FAILED %s", infoLog);
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