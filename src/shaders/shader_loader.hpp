#ifndef SRC_SHADERS_SHADER_LOADER_HPP_
#define SRC_SHADERS_SHADER_LOADER_HPP_

#include <glad/glad.h>
#include <string>

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
    // ~ShaderProgram();
};

#endif  // SRC_SHADERS_SHADER_LOADER_HPP_
