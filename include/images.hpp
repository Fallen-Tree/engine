#pragma once

#include <string>
#include "shaders.hpp"


class Image {
 private:
    ShaderProgram m_ShaderProgram;
    unsigned int m_VAO, m_VBO;
    unsigned int m_TextureID;
    float m_RelX, m_RelY;
    float m_Scale = 1.f;
    int m_Height, m_Width;
    bool m_Visible = true;

 public:
    // path - relative path to image from resources/images/
    // relX, relY - relative coordinates of left bottom corner in range [0; 1]
    // coordinates of screen also starts from left bottom corner
    // Scale
    Image() = default;
    Image(unsigned int texture, int width, int height);
    Image(std::string);
    Image(std::string, float x, float y);
    Image(std::string, float x, float y, float scale);
    // Render image
    void Render();
    // to not render picture, default
    void Hide();
    // to render picture again
    void Show();

    Image& SetScale(float);
    Image& SetRelativePosition(float, float);
    Image& SetShaderProgram(ShaderProgram);
    ShaderProgram GetShaderProgram();
};
