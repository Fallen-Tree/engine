#ifndef SRC_SHADERS_SHADER_LOADER_H_
#define SRC_SHADERS_SHADER_LOADER_H_

class Shader {
 public:
     const char* m_Source = 0;
     unsigned int m_Shader;

     int CheckSuccess(unsigned int shader);

     int LoadSourceFromFile(const char* path);

     int LoadSource(const char* source);
};

class VertexShader : public Shader {
 public:
     int Compile();
};

class FragmentShader : public Shader {
 public:
     int Compile();
};

class ShaderProgram {
 public:
     unsigned int m_Program;

     int Link(VertexShader vShader, FragmentShader fShader);
};

#endif  // SRC_SHADERS_SHADER_LOADER_H_
