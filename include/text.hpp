#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>

#include "shaders.hpp"
#include "font.hpp"

class Text {
 private:
  Font* m_Font;

  std::string m_Text;
  float m_RelX;
  float m_RelY;
  float m_Scale;
  Vec3 m_Color;

 public:
    Text(Font* font, std::string, float, float, float, Vec3);
    void SetCoordinates(float, float);
    void SetScale(float);
    void SetColor(Vec3);
    void SetRenderData(float, float, float, Vec3);
    void SetContent(std::string);
    void SetFont(Font*);
    void RenderText();
};
