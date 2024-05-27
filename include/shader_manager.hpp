#pragma once
#include <vector>
#include <string>
#include <map>
#include <utility>
#include "shaders.hpp"

class ShaderManager {
 public:
     ShaderProgram LoadShaderProgram(std::string vertexFilename, std::string fragmentFilename);
     Shader LoadShader(ShaderType type, std::string filename);
     void SetDefault(std::string vertexFilename, std::string fragmentFilename);
     void SetDefaultDepthShader(std::string vertexFilename);
     ShaderProgram &GetDefault();
     ShaderProgram &GetDefaultDepth();

     std::map<std::pair<std::string, std::string>, ShaderProgram>::iterator begin();
     std::map<std::pair<std::string, std::string>, ShaderProgram>::iterator end();
 private:
     std::vector<Shader> m_VertexShaders;
     std::vector<Shader> m_FragmentShaders;
     ShaderProgram m_DefaultShader;
     ShaderProgram m_DefaultDepthShader;
     std::map<std::pair<std::string, std::string>, ShaderProgram> m_ShaderPrograms;
};

