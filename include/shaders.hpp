#pragma once

#include <glad/glad.h>
#include <string>

#include <glm/glm.hpp>

enum ShaderType {VertexShader = GL_VERTEX_SHADER, FragmentShader = GL_FRAGMENT_SHADER};

class Shader {
 private:
     std::string m_Source;
     ShaderType m_Type;
     int CheckSuccess();
     int LoadSourceFromFile(const char* path);
     int Compile();
 public:
     unsigned int m_Shader;
     explicit Shader(ShaderType shaderType, const char* path);
};

class ShaderProgram {
 private:
     unsigned int m_Program = 0;
 public:
     int AttachShader(Shader shader);
     int Link();
    //  ShaderProgram(const ShaderProgram& other) = default;
    //  ShaderProgram(ShaderProgram&& other) = default;
    //  ShaderProgram& operator=(const ShaderProgram& other) = default;
    //  ShaderProgram& operator=(ShaderProgram&& other) = default;
     ShaderProgram();
     ShaderProgram(Shader vShader, Shader fShader);
     int Use();
     int UniformLocation(const char* mode);

     void SetVar(const char* name, const float value);
     void SetVec3(const char* name, const glm::vec3 vec);
     void SetMat4(const char* name, const glm::mat4 mat);

     ~ShaderProgram();
};
