#include "shaders.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdarg>

#include <glm/glm.hpp>

#include "config.hpp"

int Shader::CheckSuccess() {
    int success;
    char infoLog[512];
    glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_Shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return success;
}

int Shader::LoadSourceFromFile(const char* path) {
    std::ifstream shaderFile;
    char finalPath[512];
    snprintf(finalPath, sizeof(finalPath), RESOURCE_DIR"/shaders%s", path);
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        shaderFile.open(finalPath);
        std::stringstream shaderStream;
        // read file's buffer contents into stream
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        m_Source = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
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