#pragma once

#include <string>
#include "shaders.hpp"


class Image {
 private:
    ShaderProgram m_ShaderProgram;
    unsigned int m_VAO, m_VBO;
    unsigned int m_TextureID;
    float m_RelX, m_RelY;
    float m_Scale;
    int m_Height, m_Width;
    bool m_Visible = true;

 public:
    // relative path to image from resources/images/
    Image(std::string, float, float, float);
    // relative x, y from -1 to 1 each, scale
    void Render();
    // to not render picture, default
    void Hide();
    // to render picture again
    void Show();
};
