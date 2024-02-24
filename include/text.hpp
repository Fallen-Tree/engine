#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>

#include "shaders.hpp"
#include "font.hpp"

class Text {
 private:
    Font* font;

    std::string text;
    float x;
    float y;
    float scale;
    Vec3 color;

 public:
    Text() = default;
    Text(Font* font, std::string, float, float, float, Vec3);
    void SetCoordinates(float, float);
    void SetScale(float);
    void SetColor(Vec3);
    void SetRenderData(float, float, float, Vec3);
    void SetContent(std::string);
    void SetFont(Font*);
    void RenderText();
};
