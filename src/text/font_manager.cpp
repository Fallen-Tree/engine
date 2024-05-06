#include "font_manager.hpp"

FontManager::FontManager(ShaderManager &sm) : m_ShaderManager(sm) {}

Font *FontManager::LoadFont(std::string path, int size) {
    auto key = make_pair(path, size);
    if (m_Fonts.find(key) == m_Fonts.end()) {
        auto sp = m_ShaderManager.LoadShaderProgram("text.vshader", "text.fshader");
        m_Fonts[key] = Font(path, size, sp);
    }
    return &m_Fonts[key];
}
