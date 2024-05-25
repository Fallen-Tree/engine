#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>
#include "font.hpp"

class Text {
 private:
  Font* m_Font;

  std::string m_Text;
  float m_RelX = 0.5f;
  float m_RelY = 0.5f;
  float m_Scale = 1.0f;
  Vec3 m_Color;

 public:
    Text() = default;
    Text(Font* font, std::string, float, float, float, Vec3);
    Text& SetRelativePosition(float, float);
    Text& SetScale(float);
    Text& SetColor(Vec3);
    Text& SetRenderData(float, float, float, Vec3);
    Text& SetContent(std::string);
    Text& SetFont(Font*);
    void RenderText();
};
