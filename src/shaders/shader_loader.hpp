#ifndef SRC_SHADERS_SHADER_LOADER_HPP_
#define SRC_SHADERS_SHADER_LOADER_HPP_

#include <glad/glad.h>
#include <string>

enum ShaderType {UndefinedShader = 0, VertexShader = GL_VERTEX_SHADER, FragmentShader = GL_FRAGMENT_SHADER};

class Shader {
 public:
     std::string m_Source;
     unsigned int m_Shader;
     ShaderType m_Type;

     int CheckSuccess();
     int LoadSourceFromFile(const char* path);
     int Compile();
     Shader();
     explicit Shader(ShaderType shaderType, const char* path);
};

class ShaderProgram {
 public:
     unsigned int m_Program = 0;

     int AttachShader(Shader shader);
     int Link();
     ShaderProgram();
     ShaderProgram(Shader vShader, Shader fShader);
     int Use();
     int UniformLocation(const char* mode);
     ~ShaderProgram();
};

#endif  // SRC_SHADERS_SHADER_LOADER_HPP_
