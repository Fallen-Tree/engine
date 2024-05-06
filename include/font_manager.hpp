#pragma once
#include <string>
#include <map>
#include <utility>
#include "font.hpp"
#include "shader_manager.hpp"

class FontManager {
 public:
     explicit FontManager(ShaderManager &);
     Font *LoadFont(std::string filename, int size);
 private:
     ShaderManager &m_ShaderManager;
     std::map<std::pair<std::string, int>, Font> m_Fonts;
};
