
#include "shader_loader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>

int Shader::CheckSuccess(unsigned int shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(this->m_Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    return 1;
}

int Shader::LoadSourceFromFile(const char* path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        std::cout << "ERROR:SHADER::LOAD::FAILED_TO_OPEN_FILE\n" << path << std::endl;
        return 1;
    }
    fseek(f, 0, SEEK_END);
    unsigned int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    // I feel malloc is bad because we can't free const char*, but idk how to replace it with something else
    char* source = reinterpret_cast<char*>(malloc(fsize + 1));
    fread(source, fsize, 1, f);
    fclose(f);
    m_Source = source;

    return 0;
}

int Shader::LoadSource(const char* source) {
    this->m_Source = source;
    return 0;
}

int VertexShader::Compile() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &(this->m_Source), NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success = this->CheckSuccess(vertexShader);
    if (success) {
        this->m_Shader = vertexShader;
    }
    return success;
}

int FragmentShader::Compile() {
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &(this->m_Source), NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    int success = this->CheckSuccess(fragmentShader);
    if (success) {
        this->m_Shader = fragmentShader;
    }
    return success;
}

int ShaderProgram::Link(VertexShader vShader, FragmentShader fShader) {
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader.m_Shader);
    glAttachShader(shaderProgram, fShader.m_Shader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 1;
    } else {
        this->m_Program = shaderProgram;
        return 0;
    }
    // glDeleteShader(vShader);
    // glDeleteShader(fShader);
}
