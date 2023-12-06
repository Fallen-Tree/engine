#pragma once

#include <glad/glad.h>
#include <string>
#include <memory>

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
     void Free();

     ~Shader();
};

class ShaderProgram {
 private:
     unsigned int m_Program = 0;
     unsigned int GetLoc(const char* name);
     int* linkCounter = 0;
     void createLinkCounter();

 public:
     int AttachShader(const Shader &shader);
     int Link();
    //  ShaderProgram(const ShaderProgram& other) = default;
    //  ShaderProgram(ShaderProgram&& other) = default;
    //  ShaderProgram& operator=(const ShaderProgram& other) = default;
    //  ShaderProgram& operator=(ShaderProgram&& other) = default;
     ShaderProgram();
     ShaderProgram(const Shader &vShader, const Shader &fShader);
     int Use();
     int UniformLocation(const char* mode);

     void SetFloat(const char* name, const float value);
     void SetInt(const char* name, const int value);
     void SetVec2(const char* name, const glm::vec2 vec);
     void SetVec2i(const char* name, const glm::ivec2 vec);
     void SetVec3(const char* name, const glm::vec3 vec);
     void SetVec3i(const char* name, const glm::ivec3 vec);
     void SetVec4(const char* name, const glm::vec4 vec);
     void SetVec4i(const char* name, const glm::ivec4 vec);
     void SetMat3(const char* name, const glm::mat3 mat);
     void SetMat4(const char* name, const glm::mat4 mat);

     ShaderProgram(const ShaderProgram& sp) {
        m_Program = sp.m_Program;
        linkCounter = sp.linkCounter;
        (*linkCounter)++;
     }

     ~ShaderProgram();
};
