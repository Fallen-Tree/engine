#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>

#include "shaders.hpp" 


class Text {
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
    Text(const char* font, unsigned int fontSize);
    void RenderText(std::string, float, float, float, glm::vec3);
};