#include "shader_manager.hpp"
#include <string>
#include <vector>
#include <utility>

ShaderProgram ShaderManager::LoadShaderProgram(std::string vFile, std::string fFile) {
    auto pair = make_pair(vFile, fFile);
    if (m_ShaderPrograms.find(pair) == m_ShaderPrograms.end()) {
        // If not found in cache - link
        auto vShader = LoadShader(ShaderType::Vertex, vFile);
        auto fShader = LoadShader(ShaderType::Fragment, fFile);
        m_ShaderPrograms[pair] = ShaderProgram(vShader, fShader);
    }
    return m_ShaderPrograms[pair];
}

Shader ShaderManager::LoadShader(ShaderType type, std::string filename) {
    switch (type) {
        case ShaderType::Fragment:
            m_FragmentShaders.push_back(Shader(type, filename));
            return m_FragmentShaders.back();
        case ShaderType::Vertex:
            m_VertexShaders.push_back(Shader(type, filename));
            return m_VertexShaders.back();
    }
}

void ShaderManager::SetDefault(std::string vertex, std::string fragment) {
    m_DefaultShader = LoadShaderProgram(vertex, fragment);
    m_DefaultDepthShader = LoadShaderProgram(vertex, "empty.fshader");
}

ShaderProgram &ShaderManager::GetDefault() {
    return m_DefaultShader;
}

ShaderProgram &ShaderManager::GetDefaultDepth() {
    return m_DefaultDepthShader;
}

std::map<std::pair<std::string, std::string>, ShaderProgram>::iterator
    ShaderManager::begin() {
        return m_ShaderPrograms.begin();
}

std::map<std::pair<std::string, std::string>, ShaderProgram>::iterator
    ShaderManager::end() {
        return m_ShaderPrograms.end();
}
