#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>

#include "shaders.hpp"

class Font {
 private:
    ShaderProgram m_ShaderProgram;
    unsigned int m_VAO, m_VBO;
    struct Character {
       unsigned int TextureID;  // ID handle of the glyph texture
       glm::ivec2   Size;       // Size of glyph
       glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
       long int Advance;    // Offset to advance to next glyph
    };
    Character Characters[128];

 public:
    Font(const char*, unsigned int);
    void RenderText(std::string, float, float, float, glm::vec3);
};
