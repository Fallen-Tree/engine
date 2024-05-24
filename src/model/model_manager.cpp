#include "model_manager.hpp"

Model *ModelManager::LoadModel(std::string file) {
    if (m_Models.find(file) == m_Models.end()) {
        m_Models[file] = Model::loadFromFile(file);
    }
    return m_Models[file];
}

Model *ModelManager::LoadModel(std::string file, ShaderProgram *shader) {
    auto res = LoadModel(file);
    res->shader = shader;
    return res;
}
