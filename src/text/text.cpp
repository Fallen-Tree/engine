#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "text.hpp"
#include "logger.hpp"
#include "shaders.hpp"
#include "user_config.hpp"
#include "engine_config.hpp"
#include "tracy/Tracy.hpp"

void Text::RenderText() {
    ZoneScoped;
    m_Font->RenderText(m_Text, m_RelX, m_RelY, m_Scale, m_Color);
}

Text::Text(Font* font, std::string text, float x, float y, float scale, Vec3 color) {
    m_Font = font;
    m_Text = text;
    m_RelX = x;
    m_RelY = y;
    m_Scale = scale;
    m_Color = color;
}

void Text::SetCoordinates(float x, float y) {
    m_RelX = x;
    m_RelY = y;
}

void Text::SetScale(float scale) {
    m_Scale = scale;
}

void Text::SetColor(Vec3 color) {
    m_Color = color;
}

void Text::SetRenderData(float x, float y, float scale, Vec3 color) {
    m_RelX = x;
    m_RelY = y;
    m_Scale = scale;
    m_Color = color;
}

void Text::SetContent(std::string text) {
    m_Text = text;
}

void Text::SetFont(Font *font) {
    m_Font = font;
}
