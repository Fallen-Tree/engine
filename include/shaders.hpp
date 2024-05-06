#pragma once

#include <glad/glad.h>
#include <string>

#include "math_types.hpp"

enum class ShaderType {Vertex = GL_VERTEX_SHADER, Fragment = GL_FRAGMENT_SHADER};

class Shader {
 private:
     std::string m_Source;
     ShaderType m_Type;
     int CheckSuccess();
     void LoadSourceFromFile(std::string path);
     int Compile();
 public:
     unsigned int m_Shader;
     explicit Shader(ShaderType shaderType, std::string path);
     std::string GetSource();
};

class ShaderProgram {
 private:
     unsigned int m_Program = -1;
     unsigned int GetLoc(const char* name);

 public:
     int AttachShader(Shader shader);
     int Link();
     ShaderProgram() = default;
     ShaderProgram(Shader vShader, Shader fShader);
     int Use();
     int UniformLocation(const char* mode);
     bool IsValid();

     void SetFloat(const char* name, const float value);
     void SetInt(const char* name, const int value);
     void SetVec2(const char* name, const Vec2 vec);
     void SetVec2i(const char* name, const Vec2Int vec);
     void SetVec3(const char* name, const Vec3 vec);
     void SetVec3i(const char* name, const Vec3Int vec);
     void SetVec4(const char* name, const Vec4 vec);
     void SetVec4i(const char* name, const Vec4Int vec);
     void SetMat3(const char* name, const Mat3 mat);
     void SetMat4(const char* name, const Mat4 mat);
};
