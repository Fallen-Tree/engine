#pragma once
#include <string>
#include <map>
#include <utility>
#include "model.hpp"

class ModelManager {
 public:
     Model *LoadModel(std::string filename);
     Model *LoadModel(std::string filename, ShaderProgram *shader);
 private:
     std::map<std::string, Model *> m_Models;
};
