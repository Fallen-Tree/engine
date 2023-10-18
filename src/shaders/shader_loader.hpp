#ifndef SRC_SHADERS_SHADER_LOADER_HPP_
#define SRC_SHADERS_SHADER_LOADER_HPP_

#include<string>

class Shader {
 public:
     std::string m_Source;
     unsigned int m_Shader;

     int CheckSuccess();
     int LoadSourceFromFile(const char* path);
     int Compile();
};

class VertexShader : public Shader {
 public:
     int Compile();
     explicit VertexShader(const char* path);
};

class FragmentShader : public Shader {
 public:
     int Compile();
     explicit FragmentShader(const char* path);
};

class ShaderProgram {
 public:
     unsigned int m_Program = 0;

     int AttachShader(Shader shader);
     int Link();
     ShaderProgram(VertexShader vShader, FragmentShader fShader);
     int Use();
     void Delete();
};

#endif  // SRC_SHADERS_SHADER_LOADER_HPP
